#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <nng/mqtt/mqtt_client.h>
#include <nng/nng.h>
#include <nng/supplemental/util/platform.h>
#include <nng/supplemental/tls/tls.h>
#include <nng/supplemental/util/options.h>

static void
fatal(const char *msg, ...)
{
	va_list ap;
	va_start(ap, msg);
	vfprintf(stderr, msg, ap);
	va_end(ap);
	fprintf(stderr, "\n");
	exit(1);
}

#define ASSERT_NULL(p, fmt, ...)           \
	if ((p) != NULL) {                 \
		fatal(fmt, ##__VA_ARGS__); \
	}

typedef struct {
	size_t  parallel;
	uint8_t version;
	char *  url;
	bool    enable_ssl;
	char *  cacert;
	size_t  cacert_len;
	char *  cert;
	size_t  cert_len;
	char *  key;
	size_t  key_len;
	char *  keypass;
	bool    enable_sqlite;
} client_opts;

enum options {
	OPT_HELP = 1,
	OPT_VERSION,
	OPT_PARALLEL,
	OPT_URL,
	OPT_SECURE,
	OPT_CACERT,
	OPT_CERTFILE,
	OPT_KEYFILE,
	OPT_KEYPASS,
	OPT_SQLITE,
};

static nng_optspec cmd_opts[] = {
	{ .o_name = "help", .o_short = 'h', .o_val = OPT_HELP },
	{ .o_name    = "parallel",
	    .o_short = 'n',
	    .o_val   = OPT_PARALLEL,
	    .o_arg   = true },
	{ .o_name    = "version",
	    .o_short = 'V',
	    .o_val   = OPT_VERSION,
	    .o_arg   = true },
	{ .o_name = "url", .o_val = OPT_URL, .o_arg = true },
	{ .o_name = "sqlite", .o_val = OPT_SQLITE },
	{ .o_name = "secure", .o_short = 's', .o_val = OPT_SECURE },
	{ .o_name = "cacert", .o_val = OPT_CACERT, .o_arg = true },
	{ .o_name = "key", .o_val = OPT_KEYFILE, .o_arg = true },
	{ .o_name = "keypass", .o_val = OPT_KEYPASS, .o_arg = true },
	{
	    .o_name  = "cert",
	    .o_short = 'E',
	    .o_val   = OPT_CERTFILE,
	    .o_arg   = true,
	},
};

static void usage(void);
static void loadfile(const char *path, void **datap, size_t *lenp);
static int  init_dialer_tls(nng_dialer d, const char *cacert, const char *cert,
     const char *key, const char *pass);

#if defined(NNG_SUPP_SQLITE)
static int
sqlite_config(
    nng_socket *sock, nng_mqtt_sqlite_option *sqlite, uint8_t proto_ver)
{
	int rv;

	// set sqlite option
	nng_mqtt_set_sqlite_enable(sqlite, true);
	nng_mqtt_set_sqlite_flush_threshold(sqlite, 50);
	nng_mqtt_set_sqlite_max_rows(sqlite, 500);
	nng_mqtt_set_sqlite_db_dir(sqlite, "/tmp/");

	// init sqlite db
	nng_mqtt_sqlite_db_init(sqlite, "mqtt_client.db", proto_ver);

	// set sqlite option pointer to socket
	return nng_socket_set_ptr(*sock, NNG_OPT_MQTT_SQLITE, sqlite);

	return (0);
}
#endif


int
client_parse_opts(int argc, char **argv, client_opts *opt)
{
	int    idx = 1;
	char * arg;
	int    val;
	int    rv;

	while ((rv = nng_opts_parse(argc, argv, cmd_opts, &val, &arg, &idx)) ==
	    0) {
		switch (val) {
		case OPT_HELP:
			usage();
			exit(0);
			break;
		case OPT_PARALLEL:
			opt->parallel = atol(arg);
			break;
		case OPT_VERSION:
			opt->version = atol(arg);
			break;
		case OPT_URL:
			ASSERT_NULL(opt->url,
			    "URL (--url) may be specified "
			    "only once.");
			opt->url = nng_strdup(arg);
			break;
		case OPT_SQLITE:
			opt->enable_sqlite = true;
			break;
		case OPT_SECURE:
			opt->enable_ssl = true;
			break;
		case OPT_CACERT:
			ASSERT_NULL(opt->cacert,
			    "CA Certificate (--cacert) may be "
			    "specified only once.");
			loadfile(
			    arg, (void **) &opt->cacert, &opt->cacert_len);
			break;
		case OPT_CERTFILE:
			ASSERT_NULL(opt->cert,
			    "Cert (--cert) may be specified "
			    "only once.");
			loadfile(arg, (void **) &opt->cert, &opt->cert_len);
			break;
		case OPT_KEYFILE:
			ASSERT_NULL(opt->key,
			    "Key (--key) may be specified only once.");
			loadfile(arg, (void **) &opt->key, &opt->key_len);
			break;
		case OPT_KEYPASS:
			ASSERT_NULL(opt->keypass,
			    "Key Password (--keypass) may be specified only "
			    "once.");
			opt->keypass = nng_strdup(arg);
			break;
		}
	}

	switch (rv) {
	case NNG_EINVAL:
		fatal("Option %s is invalid.", argv[idx]);
		usage();
		break;
	case NNG_EAMBIGUOUS:
		fatal("Option %s is ambiguous (specify in full).", argv[idx]);
		break;
	case NNG_ENOARG:
		fatal("Option %s requires argument.", argv[idx]);
		break;
	default:
		break;
	}

	if (opt->url == NULL) {
		opt->url = nng_strdup("mqtt-tcp://127.0.0.1:1883");
	}

	if (opt->version == 0) {
		opt->version = MQTT_PROTOCOL_VERSION_v311;
	}

	if (opt->parallel == 0 ) {
		opt->parallel = 32;
	}

	return rv;
}

struct work {
	enum { INIT, RECV, WAIT, SEND } state;
	nng_aio *aio;
	nng_msg *msg;
	nng_ctx  ctx;
};

#define SUB_TOPIC1 "/nanomq/msg/1"
#define SUB_TOPIC2 "/nanomq/msg/2"
#define SUB_TOPIC3 "/nanomq/msg/3"

void
client_cb(void *arg)
{
	struct work *work = arg;
	nng_msg *    msg;
	int          rv;

	switch (work->state) {

	case INIT:
		work->state = RECV;
		nng_ctx_recv(work->ctx, work->aio);
		break;

	case RECV:
		if ((rv = nng_aio_result(work->aio)) != 0) {
			fatal("nng_recv_aio: %s", nng_strerror(rv));
			work->state = RECV;
			nng_ctx_recv(work->ctx, work->aio);
			break;
		}

		work->msg   = nng_aio_get_msg(work->aio);
		work->state = WAIT;
		nng_sleep_aio(0, work->aio);
		break;

	case WAIT:
		msg = work->msg;

		// Get PUBLISH payload and topic from msg;
		uint32_t payload_len;
		uint8_t *payload =
		    nng_mqtt_msg_get_publish_payload(msg, &payload_len);
		uint32_t    topic_len;
		const char *recv_topic =
		    nng_mqtt_msg_get_publish_topic(msg, &topic_len);

		printf("RECV: '%.*s' FROM: '%.*s'\n", payload_len,
		    (char *) payload, topic_len, recv_topic);

		uint8_t *send_data = nng_alloc(payload_len);
		memcpy(send_data, payload, payload_len);

		nng_msg_header_clear(work->msg);
		nng_msg_clear(work->msg);

		// Send payload to topic "/nanomq/msg/transfer"
		char *topic = "/nanomq/msg/transfer";
		nng_mqtt_msg_set_packet_type(work->msg, NNG_MQTT_PUBLISH);
		nng_mqtt_msg_set_publish_topic(work->msg, topic);
		nng_mqtt_msg_set_publish_payload(
		    work->msg, send_data, payload_len);

		printf("SEND: '%.*s' TO:   '%s'\n", payload_len,
		    (char *) send_data, topic);

		nng_free(send_data, payload_len);
		nng_aio_set_msg(work->aio, work->msg);
		work->msg   = NULL;
		work->state = SEND;
		nng_ctx_send(work->ctx, work->aio);
		break;

	case SEND:
		if ((rv = nng_aio_result(work->aio)) != 0) {
			nng_msg_free(work->msg);
			fatal("nng_send_aio: %s", nng_strerror(rv));
		}
		work->state = RECV;
		nng_ctx_recv(work->ctx, work->aio);
		break;

	default:
		fatal("bad state: %s", nng_strerror(NNG_ESTATE));
		break;
	}
}

struct work *
alloc_work(nng_socket sock)
{
	struct work *w;
	int          rv;

	if ((w = nng_alloc(sizeof(*w))) == NULL) {
		fatal("nng_alloc: %s", nng_strerror( NNG_ENOMEM));
	}
	if ((rv = nng_aio_alloc(&w->aio, client_cb, w)) != 0) {
		fatal("nng_aio_alloc: %s", nng_strerror(rv));
	}
	if ((rv = nng_ctx_open(&w->ctx, sock)) != 0) {
		fatal("nng_ctx_open: %s", nng_strerror(rv));
	}
	w->state = INIT;
	return (w);
}

// Connack message callback function
void
connect_cb(nng_pipe p, nng_pipe_ev ev, void *arg)
{
	int reason = 0;
	// get connect reason
	nng_pipe_get_int(p, NNG_OPT_MQTT_CONNECT_REASON, &reason);
	// get property for MQTT V5
	// property *prop;
	// nng_pipe_get_ptr(p, NNG_OPT_MQTT_CONNECT_PROPERTY, &prop);
	printf("%s: connected[%d]!\n", __FUNCTION__, reason);
}

// Disconnect message callback function
static void
disconnect_cb(nng_pipe p, nng_pipe_ev ev, void *arg)
{
	int reason = 0;
	// get connect reason
	nng_pipe_get_int(p, NNG_OPT_MQTT_DISCONNECT_REASON, &reason);
	// property *prop;
	// nng_pipe_get_ptr(p, NNG_OPT_MQTT_DISCONNECT_PROPERTY, &prop);
	printf("%s: disconnected!\n", __FUNCTION__);
}

static void
sub_callback(void *arg)
{
	nng_mqtt_client *client = (nng_mqtt_client *) arg;
	nng_aio *        aio    = client->sub_aio;
	nng_msg *        msg    = nng_aio_get_msg(aio);

	// Do not forget to judge if msg is NULL or not
	if (msg != NULL) {
		uint32_t count = 0;
		uint8_t *codes;
		codes = (uint8_t *) nng_mqtt_msg_get_suback_return_codes(
		    msg, &count);

		printf("%s: suback: ", __FUNCTION__);
		for (uint32_t i = 0; i < count; i++) {
			printf("[%d] ", codes[i]);
		}
		printf("\n");

		nng_msg_free(msg);
	}
}

int
client(client_opts *opts)
{
	nng_socket   sock;
	nng_dialer   dialer;
	struct work *works[opts->parallel];
	int          i;
	int          rv;

	rv = opts->version == MQTT_PROTOCOL_VERSION_v5
	    ? nng_mqttv5_client_open(&sock)
	    : nng_mqtt_client_open(&sock);

	printf("connecting to %s\n", opts->url);

	if (rv != 0) {
		fatal("nng_socket: %s", nng_strerror(rv));
	}

#if defined(NNG_SUPP_SQLITE)
	nng_mqtt_sqlite_option *sqlite = NULL;
	if (opts->enable_sqlite) {
		if ((rv = nng_mqtt_alloc_sqlite_opt(&sqlite)) != 0) {
			fatal(
			    "nng_mqtt_alloc_sqlite_opt: %s", nng_strerror(rv));
		}
		sqlite_config(&sock, sqlite, opts->version);
	}
#endif

	for (i = 0; i < opts->parallel; i++) {
		works[i] = alloc_work(sock);
	}

	// Mqtt connect message
	nng_msg *msg;
	nng_mqtt_msg_alloc(&msg, 0);
	nng_mqtt_msg_set_packet_type(msg, NNG_MQTT_CONNECT);
	nng_mqtt_msg_set_connect_keep_alive(msg, 60);
	nng_mqtt_msg_set_connect_clean_session(msg, true);
	nng_mqtt_msg_set_connect_proto_version(msg, opts->version);

	nng_mqtt_set_connect_cb(sock, connect_cb, &sock);
	nng_mqtt_set_disconnect_cb(sock, disconnect_cb, NULL);

	if ((rv = nng_dialer_create(&dialer, sock, opts->url)) != 0) {
		fatal("nng_dialer_create: %s", nng_strerror(rv));
	}

	if (opts->enable_ssl) {
		if ((rv = init_dialer_tls(dialer, opts->cacert, opts->cert,
		         opts->key, opts->keypass)) != 0) {
			fatal("init_dialer_tls: %s", nng_strerror(rv));
		}
	}

	nng_dialer_set_ptr(dialer, NNG_OPT_MQTT_CONNMSG, msg);
	nng_dialer_start(dialer, NNG_FLAG_NONBLOCK);

	// subscribe to mqtt broker
	nng_mqtt_topic_qos subscriptions[] = {
			{
			    .qos   = 1,
			    .topic = { 
					.buf    = (uint8_t *) SUB_TOPIC1,
			        .length = strlen(SUB_TOPIC1), 
				},
			},
			{
			    .qos   = 2,
			    .topic = { 
					.buf    = (uint8_t *) SUB_TOPIC2,
			        .length = strlen(SUB_TOPIC2), 
				},
			},
		};

	nng_mqtt_cb_opt cb_opt = {
		.sub_ack_cb = sub_callback,
	};

	nng_mqtt_client *client = nng_mqtt_client_alloc(&sock, &cb_opt, true);
	nng_mqtt_subscribe_async(client, subscriptions,
	    sizeof(subscriptions) / sizeof(nng_mqtt_topic_qos), NULL);

	for (i = 0; i < opts->parallel; i++) {
		client_cb(works[i]);
	}

	for (;;) {
		nng_msleep(3600000); // neither pause() nor sleep() portable
	}

#if defined(NNG_SUPP_SQLITE)
	nng_mqtt_free_sqlite_opt(sqlite);
#endif
}

// This reads a file into memory.  Care is taken to ensure that
// the buffer is one byte larger and contains a terminating
// NUL. (Useful for key files and such.)
static void
loadfile(const char *path, void **datap, size_t *lenp)
{
	FILE * f;
	size_t total_read      = 0;
	size_t allocation_size = BUFSIZ;
	char * fdata;
	char * realloc_result;

	if (strcmp(path, "-") == 0) {
		f = stdin;
	} else {
		if ((f = fopen(path, "rb")) == NULL) {
			fprintf(stderr, "Cannot open file %s: %s", path,
			    strerror(errno));
			exit(1);
		}
	}

	if ((fdata = malloc(allocation_size + 1)) == NULL) {
		fprintf(stderr, "Out of memory.");
	}

	while (1) {
		total_read += fread(
		    fdata + total_read, 1, allocation_size - total_read, f);
		if (ferror(f)) {
			if (errno == EINTR) {
				continue;
			}
			fprintf(stderr, "Read from %s failed: %s", path,
			    strerror(errno));
			exit(1);
		}
		if (feof(f)) {
			break;
		}
		if (total_read == allocation_size) {
			if (allocation_size > SIZE_MAX / 2) {
				fprintf(stderr, "Out of memory.");
			}
			allocation_size *= 2;
			if ((realloc_result = realloc(
			         fdata, allocation_size + 1)) == NULL) {
				free(fdata);
				fprintf(stderr, "Out of memory.");
				exit(1);
			}
			fdata = realloc_result;
		}
	}
	if (f != stdin) {
		fclose(f);
	}
	fdata[total_read] = '\0';
	*datap            = fdata;
	*lenp             = total_read;
}

static int
init_dialer_tls(nng_dialer d, const char *cacert, const char *cert,
    const char *key, const char *pass)
{
	nng_tls_config *cfg;
	int             rv;

	if ((rv = nng_tls_config_alloc(&cfg, NNG_TLS_MODE_CLIENT)) != 0) {
		return (rv);
	}

	if (cert != NULL && key != NULL) {
		nng_tls_config_auth_mode(cfg, NNG_TLS_AUTH_MODE_REQUIRED);
		if ((rv = nng_tls_config_own_cert(cfg, cert, key, pass)) !=
		    0) {
			goto out;
		}
	} else {
		nng_tls_config_auth_mode(cfg, NNG_TLS_AUTH_MODE_NONE);
	}

	if (cacert != NULL) {
		if ((rv = nng_tls_config_ca_chain(cfg, cacert, NULL)) != 0) {
			goto out;
		}
	}

	rv = nng_dialer_set_ptr(d, NNG_OPT_TLS_CONFIG, cfg);

out:
	nng_tls_config_free(cfg);
	return (rv);
}

static void
usage(void)
{
	printf("mqtt_async: \n");
	printf("    -h, --help    \n");
	printf("    --url            <url>    The url for mqtt broker \n"
	       "                     ('mqtt-tcp://host:port' or \n"
	       "                     'tls+mqtt-tcp://host:port')\n"
	       "                     [default: "
	       "mqtt-tcp://127.0.0.1:1883]\n");
	printf("    -n, --parallel   <number of works> (default: 32)\n");
	printf("    -v, --version    <mqtt version> (default: 4)\n");
	printf("    --sqlite         enable sqlite cache (default: false)\n");
	printf("    -s, --secure     enable ssl/tls mode (default: disable)\n");
	printf("    --cacert         <cafile path>\n");
	printf("    -E, --cert       <cert file path>\n");
	printf("    --key            <key file path>\n");
	printf("    --pey_pass       <key password>\n");
}

int
main(int argc, char **argv)
{
	int    rc;

	client_opts opts = { 0 };

	client_parse_opts(argc, argv, &opts);

	client(&opts);

	return 0;
}
