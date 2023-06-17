package info.mqtt.android

import android.content.Intent
import android.os.IBinder
import androidx.test.core.app.ApplicationProvider
import androidx.test.espresso.matcher.ViewMatchers.assertThat
import androidx.test.ext.junit.runners.AndroidJUnit4
import androidx.test.platform.app.InstrumentationRegistry
import androidx.test.rule.ServiceTestRule
import info.hannes.timber.DebugFormatTree
import info.mqtt.android.service.MqttAndroidClient
import info.mqtt.android.service.MqttService
import info.mqtt.android.service.MqttServiceBinder
import org.eclipse.paho.client.mqttv3.*
import org.hamcrest.CoreMatchers.`is`
import org.hamcrest.CoreMatchers.any
import org.junit.*
import org.junit.rules.TestName
import org.junit.runner.RunWith
import timber.log.Timber
import java.util.*
import java.util.concurrent.TimeUnit
import java.util.concurrent.TimeoutException
import kotlin.math.min

@RunWith(AndroidJUnit4::class)
class AndroidServiceWithActionListenerTest {

    @get:Rule
    var nameRule = TestName()

    private val classCanonicalName = this.javaClass.canonicalName
    private var serverURI: String? = null
    private var mqttSSLServerURI: String? = null
    private var waitForCompletionTime = 0L
    private var keyStorePwd = ""

    @get:Rule
    val serviceRule = ServiceTestRule()

    @Before
    @Throws(TimeoutException::class)
    fun setUp() {
        Timber.plant(DebugFormatTree())

        val properties = TestProperties(InstrumentationRegistry.getInstrumentation().targetContext)
        serverURI = properties.serverURI
        mqttSSLServerURI = properties.serverSSLURI
        waitForCompletionTime = properties.waitForCompletionTime
        keyStorePwd = properties.clientKeyStorePassword

        // Create the service Intent.
        val serviceIntent = Intent(
            ApplicationProvider.getApplicationContext(), MqttService::class.java
        ).apply {
            // putExtra(SEED_KEY, 42L)
        }

        // Bind the service and grab a reference to the binder.
        val binder: IBinder = serviceRule.bindService(serviceIntent)

        // Get the reference to the service, to call public methods on the binder directly.
        val service: MqttService = (binder as MqttServiceBinder).service

        // Verify that the service is working correctly.
        assertThat(service.isTraceEnabled, `is`(any(Boolean::class.java)))
    }

    @Test
    fun testConnect() {
        val mqttClient: IMqttAsyncClient?
        mqttClient = MqttAndroidClient(InstrumentationRegistry.getInstrumentation().targetContext, serverURI!!, "testConnect")
        var token = mqttClient.connect(null, ActionListener())
        token.waitForCompletion(waitForCompletionTime)
        token = mqttClient.disconnect(InstrumentationRegistry.getInstrumentation().targetContext, ActionListener())
        token.waitForCompletion(waitForCompletionTime)
        token = mqttClient.connect(null, ActionListener())
        token.waitForCompletion(waitForCompletionTime)
        token = mqttClient.disconnect(InstrumentationRegistry.getInstrumentation().targetContext, ActionListener())
        token.waitForCompletion(waitForCompletionTime)
    }

    @Test
    fun testRemoteConnect() {
        val mqttClient: IMqttAsyncClient?
        mqttClient = MqttAndroidClient(InstrumentationRegistry.getInstrumentation().targetContext, serverURI!!, "testRemoteConnect")
        var token = mqttClient.connect(null, ActionListener())
        token.waitForCompletion(waitForCompletionTime)
        token = mqttClient.disconnect(InstrumentationRegistry.getInstrumentation().targetContext, ActionListener())
        token.waitForCompletion(waitForCompletionTime)
        val mqttV3Receiver = MqttReceiver(mqttClient)
        mqttClient.setCallback(mqttV3Receiver)
        val mqttConnectOptions = MqttConnectOptions()
        mqttConnectOptions.isCleanSession = false
        token = mqttClient.connect(mqttConnectOptions, null, ActionListener())
        token.waitForCompletion(waitForCompletionTime)
        val topicNames = arrayOf("${nameRule.methodName}/Topic")
        val topicQos = intArrayOf(0)
        token = mqttClient.subscribe(topicNames, topicQos, null, ActionListener())
        token.waitForCompletion(waitForCompletionTime)
        val payload = "Message payload $classCanonicalName.${nameRule.methodName}".toByteArray()
        token = mqttClient.publish(topicNames[0], payload, 1, false, null, ActionListener())
        token.waitForCompletion(waitForCompletionTime)
        val validateResult = mqttV3Receiver.validateReceipt(topicNames[0], 0, payload)
        if (!validateResult.ok) {
            Assert.fail(validateResult.message)
        }
        token = mqttClient.disconnect(InstrumentationRegistry.getInstrumentation().targetContext, ActionListener())
        token.waitForCompletion(waitForCompletionTime)
    }

    @Test
    fun testLargeMessage() {
        var mqttClient: IMqttAsyncClient? = null
        try {
            mqttClient = MqttAndroidClient(InstrumentationRegistry.getInstrumentation().targetContext, serverURI!!, "testLargeMessage")
            val mqttReceiver = MqttReceiver(mqttClient)
            mqttClient.setCallback(mqttReceiver)
            var token = mqttClient.connect(null, ActionListener())
            token.waitForCompletion(waitForCompletionTime)
            val largeSize = 1000
            val topicNames = arrayOf("testLargeMessage" + "/Topic")
            val topicQos = intArrayOf(0)
            val message = ByteArray(largeSize)
            Arrays.fill(message, 's'.code.toByte())
            token = mqttClient.subscribe(topicNames, topicQos, null, ActionListener())
            token.waitForCompletion(waitForCompletionTime)
            token = mqttClient.unsubscribe(topicNames, null, ActionListener())
            token.waitForCompletion(waitForCompletionTime)
            token = mqttClient.subscribe(topicNames, topicQos, null, ActionListener())
            token.waitForCompletion(waitForCompletionTime)
            token = mqttClient.publish(topicNames[0], message, 0, false, null, ActionListener())
            token.waitForCompletion(waitForCompletionTime)
            val validateResult = mqttReceiver.validateReceipt(topicNames[0], 0, message)
            if (!validateResult.ok) {
                Assert.fail(validateResult.message)
            }
        } catch (exception: Exception) {
            Assert.fail("Failed to instantiate:${nameRule.methodName} exception=$exception")
        } finally {
            try {
                val token = mqttClient!!.disconnect(null, ActionListener())
                token.waitForCompletion(waitForCompletionTime)
                mqttClient.close()
            } catch (ignored: Exception) {
            }
        }
    }

    @Test
    fun testMultipleClients() {
        val publishers = 2
        val subscribers = 5
        val mqttPublisher = arrayOfNulls<IMqttAsyncClient>(publishers)
        val mqttSubscriber = arrayOfNulls<IMqttAsyncClient>(subscribers)
        var disconnectToken: IMqttToken
        val topicNames = arrayOf("${nameRule.methodName}/Topic")
        val topicQos = intArrayOf(0)
        for (i in mqttPublisher.indices) {
            mqttPublisher[i] = MqttAndroidClient(InstrumentationRegistry.getInstrumentation().targetContext, serverURI!!, "MultiPub$i")
            val token = mqttPublisher[i]!!.connect(null, ActionListener())
            token.waitForCompletion(waitForCompletionTime)
        }
        val mqttReceiver = arrayOfNulls<MqttReceiver>(mqttSubscriber.size)
        for (i in mqttSubscriber.indices) {
            mqttSubscriber[i] = MqttAndroidClient(InstrumentationRegistry.getInstrumentation().targetContext, serverURI!!, "MultiSubscriber$i")
            mqttReceiver[i] = MqttReceiver(mqttSubscriber[i]!!)
            mqttSubscriber[i]!!.setCallback(mqttReceiver[i]!!)
            var token = mqttSubscriber[i]!!.connect(null, ActionListener())
            token.waitForCompletion(waitForCompletionTime)
            token = mqttSubscriber[i]!!.subscribe(topicNames, topicQos, null, ActionListener())
            token.waitForCompletion(waitForCompletionTime)
        }
        for (iMessage in 0..1) {
            val payload = "Message $iMessage".toByteArray()
            for (aMqttPublisher in mqttPublisher) {
                val token = aMqttPublisher!!.publish(topicNames[0], payload, 0, false, null, ActionListener())
                token.waitForCompletion(waitForCompletionTime)
            }
            for (i in mqttSubscriber.indices) {
                repeat(mqttPublisher.count()) {
                    val validateResult = mqttReceiver[i]!!.validateReceipt(topicNames[0], 0, payload)
                    if (!validateResult.ok) {
                        Assert.fail("$mqttPublisher ${validateResult.message}")
                    }
                }
            }
        }
        mqttPublisher.forEach {
            disconnectToken = it!!.disconnect(null, null)
            disconnectToken.waitForCompletion(waitForCompletionTime)
            it.close()
        }
        mqttSubscriber.forEach {
            disconnectToken = it!!.disconnect(null, null)
            disconnectToken.waitForCompletion(waitForCompletionTime)
            it.close()
        }
    }

    @Test
    @Ignore("Received invalid payload")
    fun testNonDurableSubs() {
        var mqttClient: IMqttAsyncClient?
        val connectToken: IMqttToken
        mqttClient = MqttAndroidClient(InstrumentationRegistry.getInstrumentation().targetContext, serverURI!!, "testNonDurableSubs")
        var mqttReceiver = MqttReceiver(mqttClient)
        mqttClient.setCallback(mqttReceiver)
        var mqttConnectOptions = MqttConnectOptions()
        // Clean session true is the default and implies non durable subscriptions.
        mqttConnectOptions.isCleanSession = true
        var token = mqttClient.connect(mqttConnectOptions, null, ActionListener())
        token.waitForCompletion(waitForCompletionTime)
        val topicNames = arrayOf("${nameRule.methodName}/Topic")
        val topicQos = intArrayOf(2)
        token = mqttClient.subscribe(topicNames, topicQos, null, ActionListener())
        token.waitForCompletion(waitForCompletionTime)
        val payloadNotRetained = "Message payload $classCanonicalName.${nameRule.methodName} not retained".toByteArray()
        token = mqttClient.publish(topicNames[0], payloadNotRetained, 2, false, null, ActionListener())
        token.waitForCompletion(waitForCompletionTime)
        var validateResult = mqttReceiver.validateReceipt(topicNames[0], 2, payloadNotRetained)
        if (!validateResult.ok) {
            Assert.fail(validateResult.message)
        }

        // Retained publications.
        // ----------------------
        val payloadRetained = "Message payload $classCanonicalName.${nameRule.methodName} retained".toByteArray()
        token = mqttClient.publish(topicNames[0], payloadRetained, 2, true, null, ActionListener())
        token.waitForCompletion(waitForCompletionTime)
        validateResult = mqttReceiver.validateReceipt(topicNames[0], 2, payloadRetained)
        if (!validateResult.ok) {
            Assert.fail(validateResult.message)
        }

        // Check that unsubscribe and re subscribe resends the publication.
        token = mqttClient.unsubscribe(topicNames, null, ActionListener())
        token.waitForCompletion(waitForCompletionTime)
        token = mqttClient.subscribe(topicNames, topicQos, null, ActionListener())
        token.waitForCompletion(waitForCompletionTime)
        validateResult = mqttReceiver.validateReceipt(topicNames[0], 2, payloadRetained)
        if (!validateResult.ok) {
            Assert.fail(validateResult.message)
        }

        // Check that subscribe without unsubscribe receives the
        // publication.
        token = mqttClient.subscribe(topicNames, topicQos, null, ActionListener())
        token.waitForCompletion(waitForCompletionTime)
        validateResult = mqttReceiver.validateReceipt(topicNames[0], 2, payloadRetained)
        if (!validateResult.ok) {
            Assert.fail(validateResult.message)
        }

        // Disconnect, reconnect and check that the retained publication is still delivered.
        token = mqttClient.disconnect(InstrumentationRegistry.getInstrumentation().targetContext, ActionListener())
        token.waitForCompletion(waitForCompletionTime)
        mqttClient.close()
        mqttClient = MqttAndroidClient(InstrumentationRegistry.getInstrumentation().targetContext, serverURI!!, "testNonDurableSubs")
        mqttReceiver = MqttReceiver(mqttClient)
        mqttClient.setCallback(mqttReceiver)
        mqttConnectOptions = MqttConnectOptions()
        mqttConnectOptions.isCleanSession = true
        connectToken = mqttClient.connect(mqttConnectOptions, null, ActionListener())
        connectToken.waitForCompletion(1000)
        token = mqttClient.subscribe(topicNames, topicQos, null, ActionListener())
        token.waitForCompletion(waitForCompletionTime)
        validateResult = mqttReceiver.validateReceipt(topicNames[0], 2, payloadRetained)
        if (!validateResult.ok) {
            Assert.fail(validateResult.message)
        }
        token = mqttClient.disconnect(InstrumentationRegistry.getInstrumentation().targetContext, ActionListener())
        token.waitForCompletion(waitForCompletionTime)
        mqttClient.close()
    }

    @Suppress
    @Test
    fun testQoSPreserved() {
        val mqttClient: IMqttAsyncClient
        mqttClient = MqttAndroidClient(InstrumentationRegistry.getInstrumentation().targetContext, serverURI!!, "testQoSPreserved")
        val mqttReceiver = MqttReceiver(mqttClient)
        mqttClient.setCallback(mqttReceiver)
        var token = mqttClient.connect(null, ActionListener())
        token.waitForCompletion(waitForCompletionTime)
        val topicNames = arrayOf("${nameRule.methodName}/Topic0", "${nameRule.methodName}/Topic1", "${nameRule.methodName}/Topic2")
        val topicQos = intArrayOf(0, 1, 2)
        token = mqttClient.subscribe(topicNames, topicQos, null, ActionListener())
        token.waitForCompletion(waitForCompletionTime)
        for (i in topicNames.indices) {
            val message = ("Message payload " + classCanonicalName + ".${nameRule.methodName} " + topicNames[i]).toByteArray()
            for (iQos in 0..2) {
                token = mqttClient.publish(topicNames[i], message, iQos, false, null, null)
                token.waitForCompletion(waitForCompletionTime)
                val validateResult = mqttReceiver.validateReceipt(topicNames[i], min(iQos, topicQos[i]), message)
                if (!validateResult.ok) {
                    Assert.fail("Receive failed sub Qos=" + topicQos[i] + " PublishQos=" + iQos)
                }
            }
        }
        token = mqttClient.disconnect(InstrumentationRegistry.getInstrumentation().targetContext, ActionListener())
        token.waitForCompletion(waitForCompletionTime)
    }

    @Test
    fun testHAConnect() {
        var client: IMqttAsyncClient? = null
        try {
            try {
                val junk = "tcp://junk:123"
                client = MqttAndroidClient(InstrumentationRegistry.getInstrumentation().targetContext, junk, nameRule.methodName)
                val urls = arrayOf("tcp://junk", serverURI)
                val options = MqttConnectOptions()
                options.serverURIs = urls
                Timber.i("HA connect")
                var token = client.connect(options, null, ActionListener())
                token.waitForCompletion(waitForCompletionTime)
                Timber.i("HA disconnect")
                token = client.disconnect(InstrumentationRegistry.getInstrumentation().targetContext, ActionListener())
                token.waitForCompletion(waitForCompletionTime)
                Timber.i("HA success")
            } catch (e: Exception) {
                e.printStackTrace()
                throw e
            }
        } finally {
            client?.close()
        }
    }

    @Test
    fun testPubSub() {
        var mqttClient: IMqttAsyncClient? = null
        try {
            mqttClient = MqttAndroidClient(InstrumentationRegistry.getInstrumentation().targetContext, serverURI!!, nameRule.methodName)
            val mqttReceiver = MqttReceiver(mqttClient)
            mqttClient.setCallback(mqttReceiver)
            var token = mqttClient.connect(null, ActionListener())
            token.waitForCompletion(waitForCompletionTime)
            val topicNames = arrayOf("testPubSub" + "/Topic")
            val topicQos = intArrayOf(0)
            val mqttMessage = MqttMessage("message for testPubSub".toByteArray())
            val message = mqttMessage.payload
            token = mqttClient.subscribe(topicNames, topicQos, null, ActionListener())
            token.waitForCompletion(waitForCompletionTime)
            token = mqttClient.publish(topicNames[0], message, 0, false, null, ActionListener())
            token.waitForCompletion(waitForCompletionTime)
            TimeUnit.MILLISECONDS.sleep(3000)
            val validateResult = mqttReceiver.validateReceipt(topicNames[0], 0, message)
            if (!validateResult.ok) {
                Assert.fail(validateResult.message)
            }
        } catch (exception: Exception) {
            Assert.fail("Failed to instantiate:${nameRule.methodName} exception=$exception")
        } finally {
            try {
                val token = mqttClient!!.disconnect(null, ActionListener())
                token.waitForCompletion(waitForCompletionTime)
                mqttClient.close()
            } catch (ignored: Exception) {
            }
        }
    }

    @Test
    fun testRetainedMessage() {
        val mqttClient: IMqttAsyncClient?
        val mqttClientRetained: IMqttAsyncClient?
        try {
            mqttClient = MqttAndroidClient(InstrumentationRegistry.getInstrumentation().targetContext, serverURI!!, nameRule.methodName)
            val mqttV3Receiver = MqttReceiver(mqttClient)
            mqttClient.setCallback(mqttV3Receiver)
            var token = mqttClient.connect(null, ActionListener())
            token.waitForCompletion(waitForCompletionTime)
            val topicNames = arrayOf("testRetainedMessage" + "/Topic")
            val topicQos = intArrayOf(0)
            val mqttMessage = MqttMessage("message for testPubSub".toByteArray())
            val message = mqttMessage.payload
            token = mqttClient.subscribe(topicNames, topicQos, null, ActionListener())
            token.waitForCompletion(waitForCompletionTime)
            token = mqttClient.publish(topicNames[0], message, 0, true, null, ActionListener())
            token.waitForCompletion(waitForCompletionTime)
            TimeUnit.MILLISECONDS.sleep(3000)
            var validateResult = mqttV3Receiver.validateReceipt(topicNames[0], 0, message)
            if (!validateResult.ok) {
                Assert.fail(validateResult.message)
            }
            Timber.i("First client received message successfully")
            token = mqttClient.disconnect(InstrumentationRegistry.getInstrumentation().targetContext, ActionListener())
            token.waitForCompletion(waitForCompletionTime)
            mqttClient.close()
            mqttClientRetained = MqttAndroidClient(InstrumentationRegistry.getInstrumentation().targetContext, serverURI!!, "Retained")
            Timber.i("New MqttAndroidClient mqttClientRetained")
            val mqttReceiver = MqttReceiver(mqttClientRetained)
            mqttClientRetained.setCallback(mqttReceiver)
            Timber.i("Assigning callback...")
            token = mqttClientRetained.connect(null, ActionListener())
            token.waitForCompletion(waitForCompletionTime)
            Timber.i("Connect to mqtt server")
            token = mqttClientRetained.subscribe(topicNames, topicQos, null, ActionListener())
            token.waitForCompletion(waitForCompletionTime)
            Timber.i("subscribe " + topicNames[0] + " QoS is " + topicQos[0])
            TimeUnit.MILLISECONDS.sleep(3000)
            validateResult = mqttReceiver.validateReceipt(topicNames[0], 0, message)
            if (!validateResult.ok) {
                Assert.fail(validateResult.message)
            }
            Timber.i("Second client received message successfully")
            token = mqttClientRetained.disconnect(InstrumentationRegistry.getInstrumentation().targetContext, ActionListener())
            token.waitForCompletion(waitForCompletionTime)
            mqttClientRetained.close()
        } catch (exception: Exception) {
            Assert.fail("Failed to instantiate:${nameRule.methodName} exception=$exception")
        }
    }

    /**
     * Tests that a client can be constructed and that it can connect to and disconnect from the service via SSL
     */
    @Ignore("Trust anchor for certification path not found")
    @Test
    fun testSSLConnect() {
        var mqttClient: MqttAndroidClient? = null
        try {
            mqttClient = MqttAndroidClient(InstrumentationRegistry.getInstrumentation().targetContext, mqttSSLServerURI!!, "testSSLConnect")
            val options = MqttConnectOptions()
            options.socketFactory =
                mqttClient.getSSLSocketFactory(InstrumentationRegistry.getInstrumentation().targetContext.assets.open("test.bks"), keyStorePwd)
            var connectToken = mqttClient.connect(options, InstrumentationRegistry.getInstrumentation().targetContext, ActionListener())
            connectToken.waitForCompletion(waitForCompletionTime)
            var disconnectToken: IMqttToken = mqttClient.disconnect(InstrumentationRegistry.getInstrumentation().targetContext, ActionListener())
            disconnectToken.waitForCompletion(waitForCompletionTime)
            connectToken = mqttClient.connect(options, InstrumentationRegistry.getInstrumentation().targetContext, ActionListener())
            connectToken.waitForCompletion(waitForCompletionTime)
            disconnectToken = mqttClient.disconnect(InstrumentationRegistry.getInstrumentation().targetContext, ActionListener())
            disconnectToken.waitForCompletion(waitForCompletionTime)
        } catch (exception: Exception) {
            Assert.fail("Failed:testSSLConnect exception=$exception")
        } finally {
            mqttClient?.close()
        }
    }

    /**
     * An SSL connection with server cert authentication, simple pub/sub of an message
     */
    @Ignore("Trust anchor for certification path not found")
    @Test
    fun testSSLPubSub() {
        var mqttClient: MqttAndroidClient? = null
        val connectToken: IMqttToken?
        val disconnectToken: IMqttToken?
        val subToken: IMqttToken?
        val pubToken: IMqttDeliveryToken?
        try {
            mqttClient = MqttAndroidClient(InstrumentationRegistry.getInstrumentation().targetContext, mqttSSLServerURI!!, "testSSLPubSub")
            val options = MqttConnectOptions()
            options.socketFactory =
                mqttClient.getSSLSocketFactory(InstrumentationRegistry.getInstrumentation().targetContext.assets.open("test.bks"), keyStorePwd)
            val mqttReceiver = MqttReceiver(mqttClient)
            mqttClient.setCallback(mqttReceiver)
            connectToken = mqttClient.connect(options, InstrumentationRegistry.getInstrumentation().targetContext, ActionListener())
            connectToken.waitForCompletion(waitForCompletionTime)
            val topicNames = arrayOf("testSSLPubSub" + "/Topic")
            val topicQos = intArrayOf(0)
            val mqttMessage = MqttMessage("message for testSSLPubSub".toByteArray())
            val message = mqttMessage.payload
            subToken = mqttClient.subscribe(topicNames, topicQos, null, ActionListener())
            subToken.waitForCompletion(waitForCompletionTime)
            pubToken = mqttClient.publish(topicNames[0], message, 0, false, null, ActionListener())
            pubToken.waitForCompletion(waitForCompletionTime)
            TimeUnit.MILLISECONDS.sleep(6000)
            val validateResult = mqttReceiver.validateReceipt(topicNames[0], 0, message)
            if (!validateResult.ok) {
                Assert.fail(validateResult.message)
            }
        } catch (exception: Exception) {
            Assert.fail("Failed:testSSLPubSub exception=$exception")
        } finally {
            disconnectToken = mqttClient!!.disconnect(InstrumentationRegistry.getInstrumentation().targetContext, ActionListener())
            disconnectToken.waitForCompletion(waitForCompletionTime)
            mqttClient.close()
        }
    }

    private inner class ActionListener : IMqttActionListener {

        override fun onFailure(token: IMqttToken?, exception: Throwable?) {
            Assert.fail("onFailure $exception")
        }

        override fun onSuccess(token: IMqttToken) {
            Timber.d("onSuccess")
        }
    }

    companion object {
        private val classCanonicalName = this::class.java.canonicalName
        private val TAG = classCanonicalName
    }
}