package io.emqx.mqtt;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileReader;
import java.security.KeyFactory;
import java.security.KeyPair;
import java.security.KeyStore;
import java.security.PrivateKey;
import java.security.Security;
import java.security.cert.Certificate;
import java.security.cert.CertificateFactory;
import java.security.cert.X509Certificate;
import java.security.spec.KeySpec;
import java.security.spec.PKCS8EncodedKeySpec;
import java.text.MessageFormat;
import java.util.Arrays;

import javax.net.ssl.KeyManagerFactory;
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLSocketFactory;
import javax.net.ssl.TrustManagerFactory;

import org.bouncycastle.asn1.pkcs.PrivateKeyInfo;
import org.bouncycastle.jce.provider.BouncyCastleProvider;
import org.bouncycastle.openssl.PEMKeyPair;
import org.bouncycastle.openssl.PEMParser;
import org.bouncycastle.openssl.jcajce.JcaPEMKeyConverter;
import org.bouncycastle.pkcs.PKCS8EncryptedPrivateKeyInfo;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;

public class MqttTwoWayTlsSample {
	
	private static final String BROKER = "broker.emqx.io";
	private static final String PORT = "8883";
	private static final String CLIENT_ID = MqttClient.generateClientId();
	private static final String USERNAME = "emqx";
	private static final String PASSWORD = "public";
	private static final int CONNECT_TIMEOUT = 300;
	private static final boolean CLEAN_SESSION = true;
	private static final String TOPIC = "java-mqtt/tls";
	private static final int QoS = 1;
	private static final String PAYLOAD = "Enjoy the sample";
	
	private static final String CA_CERT_PATH = "./cacert.pem";
	private static final String CLIENT_CERT_PATH = "./client-cert.pem";
	private static final String CLIENT_KEY_PATH = "./client-key.pem";
	
	public static void main (String args[]) {
		MqttClient client = null;
		try {
			String server = "ssl://" + BROKER + ":" + PORT;
			client = new MqttClient(server, CLIENT_ID);
			
			client.setCallback(new MqttCallback() {

				@Override
				public void connectionLost(Throwable cause) {
					System.out.println(MessageFormat.format("Connection lost. Cause: {0}", cause));
				}

				@Override
				public void messageArrived(String topic, MqttMessage message) throws Exception {
					System.out.println(MessageFormat.format("Callback: received message from topic {0}: {1}", 
							topic, message.toString()));
				}

				@Override
				public void deliveryComplete(IMqttDeliveryToken token) {
					try {
						System.out.println(MessageFormat.format("Callback: delivered message to topics {0}", 
								Arrays.asList(token.getTopics())));
					} catch (Exception e) {
						e.printStackTrace();
					}
				}
				
			});
			
			MqttConnectOptions options = new MqttConnectOptions();
			options.setUserName(USERNAME);
			options.setPassword(PASSWORD.toCharArray());
			options.setConnectionTimeout(CONNECT_TIMEOUT);
			options.setCleanSession(CLEAN_SESSION);
			options.setSocketFactory(getSocketFactory(CA_CERT_PATH, CLIENT_CERT_PATH, CLIENT_KEY_PATH));
			
			System.out.println("Connecting to broker: " + server);
			client.connect(options);
			
			if (!client.isConnected()) {
				System.out.println("Failed to connect to broker: " + server);
				return;
			}
			System.out.println("Connected to broker: " + server);
			
			client.subscribe(TOPIC, QoS);
			System.out.println("Subscribed to topic: " + TOPIC);
			
			MqttMessage msg = new MqttMessage(PAYLOAD.getBytes("UTF-8"));
			msg.setQos(QoS);
			client.publish(TOPIC, msg);
			
			System.out.println("Disconnect from broker: " + server);
			client.disconnect();
		} catch (Exception ex) {
			ex.printStackTrace();
		} finally {
			if (client != null) {
				try {
					client.close();
				} catch (MqttException e) {
					e.printStackTrace();
				}
			}
		}
	}
	
	 public static SSLSocketFactory getSocketFactory(String caCertPath, String clientCertPath, String clientKeyPath) throws Exception {
		 CertificateFactory certFactory = CertificateFactory.getInstance("X.509");
		 
		 // load CA certificate into keystore to authenticate server
		 Certificate caCert = certFactory.generateCertificate(new FileInputStream(caCertPath));
		 X509Certificate x509CaCert = (X509Certificate) caCert;
		 
		 KeyStore caKeyStore = KeyStore.getInstance(KeyStore.getDefaultType());
		 caKeyStore.load(null, null);
		 caKeyStore.setCertificateEntry("cacert", x509CaCert);
		 
		 TrustManagerFactory tmFactory = TrustManagerFactory.getInstance(TrustManagerFactory.getDefaultAlgorithm());
		 tmFactory.init(caKeyStore);
		 
		 // load client certificate and private key to be used by server to authenticate client
		 Certificate clientCert = certFactory.generateCertificate(new FileInputStream(clientCertPath));
		 X509Certificate x509ClientCert = (X509Certificate) clientCert;
		 
		 String password = "keypass";
		 
		 KeyStore clientKeyStore = KeyStore.getInstance(KeyStore.getDefaultType());
		 clientKeyStore.load(null, null);
		 clientKeyStore.setCertificateEntry("cert", x509ClientCert);
		 clientKeyStore.setKeyEntry("key", loadKey(new File(clientKeyPath)), password.toCharArray(), new Certificate[] {x509ClientCert});
		 
		 KeyManagerFactory kmFactory = KeyManagerFactory.getInstance(KeyManagerFactory.getDefaultAlgorithm());
		 kmFactory.init(clientKeyStore, password.toCharArray());
		 
		 SSLContext sslContext = SSLContext.getInstance("TLSv1.2");
		 sslContext.init(kmFactory.getKeyManagers(), tmFactory.getTrustManagers(), null);
		 
		 return sslContext.getSocketFactory();
	 }
	 
	 /*
	  * Load private key from PEM with help of bouncy castle
	  * RSA/DSA private keys are supported
	  */
	 public static PrivateKey loadKey(File keyFile) throws Exception {
		 PEMParser pemParser = null;
		 PrivateKey key = null;
		 String bcProvider = "BC";
		 try {
			 if (Security.getProvider(bcProvider) == null) {
				 Security.addProvider(new BouncyCastleProvider()); 
			 }
			 
			 pemParser = new PEMParser(new FileReader(keyFile));
			 Object keyObj = pemParser.readObject();
			 if (keyObj instanceof PEMKeyPair) {
				 JcaPEMKeyConverter converter = new JcaPEMKeyConverter().setProvider(bcProvider);
				 KeyPair keyPair = converter.getKeyPair((PEMKeyPair) keyObj);
				 key = keyPair.getPrivate();
			 } else {
				 byte[] keyEncoded = null;
				 if (keyObj instanceof PKCS8EncryptedPrivateKeyInfo) {
					 keyEncoded = ((PKCS8EncryptedPrivateKeyInfo) keyObj).getEncoded();
				 } else if (keyObj instanceof PrivateKeyInfo) {
					 keyEncoded = ((PrivateKeyInfo) keyObj).getEncoded();
				 }
				 if (keyEncoded != null) {
					 KeyFactory keyFactory = KeyFactory.getInstance("RSA");
					 KeySpec keySpec = new PKCS8EncodedKeySpec(keyEncoded);
					 key = keyFactory.generatePrivate(keySpec);
				 }
			 }
		 } catch (Exception e) {
			 throw e;
		 } finally {
			 if (pemParser != null) {
				 pemParser.close();
			 }
		 }
		 return key;
	 }

}
