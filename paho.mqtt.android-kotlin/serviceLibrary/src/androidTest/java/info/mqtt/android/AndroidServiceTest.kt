package info.mqtt.android

import androidx.test.ext.junit.runners.AndroidJUnit4
import androidx.test.platform.app.InstrumentationRegistry
import info.hannes.timber.DebugFormatTree
import info.mqtt.android.service.MqttAndroidClient
import org.eclipse.paho.client.mqttv3.*
import org.junit.Assert.*
import org.junit.Before
import org.junit.Ignore
import org.junit.Rule
import org.junit.Test
import org.junit.rules.TestName
import org.junit.runner.RunWith
import timber.log.Timber
import java.util.*
import java.util.concurrent.TimeUnit
import kotlin.math.min

@RunWith(AndroidJUnit4::class)
class AndroidServiceTest : IMqttActionListener {

    @get:Rule
    var nameRule = TestName()

    private var mqttServerURI: String? = null
    private var mqttSSLServerURI: String? = null
    private var waitForCompletionTime = 700L
    private lateinit var keyStorePwd: String

    @Before
    fun setUp() {
        Timber.plant(DebugFormatTree())

        val properties = TestProperties(InstrumentationRegistry.getInstrumentation().targetContext)
        mqttServerURI = properties.serverURI
        mqttSSLServerURI = properties.serverSSLURI
        waitForCompletionTime = properties.waitForCompletionTime
        keyStorePwd = properties.clientKeyStorePassword
        Timber.d(properties.serverSSLURI)
    }

    /**
     * Tests that a client can be constructed and that it can connect to and disconnect from the service
     */
    @Test
    fun testConnect() {
        try {
            MqttAndroidClient(InstrumentationRegistry.getInstrumentation().targetContext, mqttServerURI!!, "testConnect").use { mqttClient ->
                var connectToken: IMqttToken = mqttClient.connect(null, null)
                connectToken.waitForCompletion(waitForCompletionTime)
                var disconnectToken: IMqttToken = mqttClient.disconnect(InstrumentationRegistry.getInstrumentation().targetContext, this)
                disconnectToken.waitForCompletion(waitForCompletionTime)
                connectToken = mqttClient.connect(null, null)
                connectToken.waitForCompletion(waitForCompletionTime)
                disconnectToken = mqttClient.disconnect(InstrumentationRegistry.getInstrumentation().targetContext, this)
                disconnectToken.waitForCompletion(waitForCompletionTime)
            }
        } catch (exception: Exception) {
            fail("Failed: $mqttServerURI exception=${exception.stackTraceToString()}")
        }
    }

    /**
     * Tests that a client connection with cleanSession=False receives the session Present Flag in
     * a subsequent connection.
     *
     * 1. Connect with CleanSession=True to ensure that state is cleared.
     * 2. Connect with CleanSession=False and ensure that sessionPresent is false.
     * 3. Connect with CleanSession=False and ensure that sessionPresent is true.
     */
    @Test
    fun testCleanSession() {
        try {
            MqttAndroidClient(
                InstrumentationRegistry.getInstrumentation().targetContext,
                mqttServerURI!!,
                "testConnectWithCleanSession"
            ).use { mqttClient ->
                var connectToken: IMqttToken
                val options1 = MqttConnectOptions()
                options1.isCleanSession = true
                val connectCallback1 = MqttConnectCallback()
                connectToken = mqttClient.connect(options1, null, connectCallback1)
                connectToken.waitForCompletion(waitForCompletionTime)
                val connectedToken1 = connectCallback1.asyncActionToken
                assertFalse(connectedToken1!!.sessionPresent)
                var disconnectToken: IMqttToken = mqttClient.disconnect(InstrumentationRegistry.getInstrumentation().targetContext, this)
                disconnectToken.waitForCompletion(waitForCompletionTime)
                val options2 = MqttConnectOptions()
                options2.isCleanSession = false
                val connectCallback2 = MqttConnectCallback()
                connectToken = mqttClient.connect(options2, null, connectCallback2)
                connectToken.waitForCompletion(waitForCompletionTime)
                val connectedToken2 = connectCallback2.asyncActionToken
                assertFalse(connectedToken2!!.sessionPresent)
                disconnectToken = mqttClient.disconnect(InstrumentationRegistry.getInstrumentation().targetContext, this)
                disconnectToken.waitForCompletion(waitForCompletionTime)
                val connectCallback3 = MqttConnectCallback()
                connectToken = mqttClient.connect(options2, null, connectCallback3)
                connectToken.waitForCompletion(waitForCompletionTime)
                val connectedToken3 = connectCallback3.asyncActionToken
                assertTrue(connectedToken3!!.sessionPresent)
                disconnectToken = mqttClient.disconnect(InstrumentationRegistry.getInstrumentation().targetContext, this)
                disconnectToken.waitForCompletion(waitForCompletionTime)
            }
        } catch (exception: Exception) {
            fail("Failed: $mqttServerURI exception=${exception.stackTraceToString()}")
        }
    }

    /**
     * Tests isConnected() returns false after a disconnect() call.
     */
    @Throws(Exception::class)
    fun testIsConnected() {
        MqttAndroidClient(InstrumentationRegistry.getInstrumentation().targetContext, mqttServerURI!!, "testConnect").use { mqttClient ->
            assertFalse(mqttClient.isConnected)
            val connectToken: IMqttToken = mqttClient.connect(null, null)
            connectToken.waitForCompletion(waitForCompletionTime)
            assertTrue(mqttClient.isConnected)
            val disconnectToken: IMqttToken = mqttClient.disconnect(InstrumentationRegistry.getInstrumentation().targetContext, this)
            disconnectToken.waitForCompletion(waitForCompletionTime)
            assertFalse(mqttClient.isConnected)
        }
    }

    /**
     * Test connection using a remote host name for the local host.
     */
    @Test
    fun testRemoteConnect() {
        try {
            MqttAndroidClient(InstrumentationRegistry.getInstrumentation().targetContext, mqttServerURI!!, "testRemoteConnect").use { mqttClient ->
                val subToken: IMqttToken
                val pubToken: IMqttDeliveryToken
                var connectToken: IMqttToken = mqttClient.connect(null, null)
                connectToken.waitForCompletion(waitForCompletionTime)
                var disconnectToken: IMqttToken = mqttClient.disconnect(InstrumentationRegistry.getInstrumentation().targetContext, this)
                disconnectToken.waitForCompletion(waitForCompletionTime)
                val mqttReceiver = MqttReceiver(mqttClient)
                mqttClient.setCallback(mqttReceiver)
                val mqttConnectOptions = MqttConnectOptions()
                mqttConnectOptions.isCleanSession = false
                connectToken = mqttClient.connect(mqttConnectOptions, null, null)
                connectToken.waitForCompletion(waitForCompletionTime)
                val topicNames = arrayOf("${nameRule.methodName}/Topic")
                val topicQos = intArrayOf(0)
                subToken = mqttClient.subscribe(topicNames, topicQos, null, null)
                subToken.waitForCompletion(waitForCompletionTime)
                val payload = "Message payload $classSimpleName.${nameRule.methodName}"
                    .toByteArray()
                pubToken = mqttClient.publish(topicNames[0], payload, 1, false, null, null)
                pubToken.waitForCompletion(waitForCompletionTime)
                val validateResult = mqttReceiver.validateReceipt(topicNames[0], 0, payload)
                if (!validateResult.ok) {
                    fail(validateResult.message)
                }
                disconnectToken = mqttClient.disconnect(InstrumentationRegistry.getInstrumentation().targetContext, this)
                disconnectToken.waitForCompletion(waitForCompletionTime)
            }
        } catch (exception: Exception) {
            fail("Failed: $mqttServerURI exception=${exception.stackTraceToString()}")
        }
    }

    /**
     * Test client pubSub using very large messages
     */
    @Test
    fun testLargeMessage() {
        var mqttClient: IMqttAsyncClient? = null
        try {
            mqttClient = MqttAndroidClient(InstrumentationRegistry.getInstrumentation().targetContext, mqttServerURI!!, "testLargeMessage")
            var subToken: IMqttToken
            val unSubToken: IMqttToken
            val pubToken: IMqttDeliveryToken
            val mqttReceiver = MqttReceiver(mqttClient) //TODO do something about this?
            mqttClient.setCallback(mqttReceiver)
            val connectToken: IMqttToken = mqttClient.connect(null, null)
            connectToken.waitForCompletion(waitForCompletionTime)
            val largeSize = 1000
            val topicNames = arrayOf("testLargeMessage" + "/Topic")
            val topicQos = intArrayOf(0)
            val message = ByteArray(largeSize)
            Arrays.fill(message, 's'.code.toByte())
            subToken = mqttClient.subscribe(topicNames, topicQos, null, null)
            subToken.waitForCompletion(waitForCompletionTime)
            unSubToken = mqttClient.unsubscribe(topicNames, null, null)
            unSubToken.waitForCompletion(waitForCompletionTime)
            subToken = mqttClient.subscribe(topicNames, topicQos, null, null)
            subToken.waitForCompletion(waitForCompletionTime)
            pubToken = mqttClient.publish(topicNames[0], message, 0, false, null, null)
            pubToken.waitForCompletion(waitForCompletionTime)
            val validateResult = mqttReceiver.validateReceipt(topicNames[0], 0, message)
            if (!validateResult.ok) {
                fail(validateResult.message)
            }
        } catch (exception: Exception) {
            fail("Failed ${nameRule.methodName} exception=${exception.stackTraceToString()}")
        } finally {
            try {
                val disconnectToken = mqttClient!!.disconnect(null, null)
                disconnectToken.waitForCompletion(waitForCompletionTime)
                mqttClient.close()
            } catch (ignored: Exception) {
            }
        }
    }

    /**
     * Multiple publishers and subscribers.
     */
    @Suppress
    @Test
    fun testMultipleClients() {
        val publishers = 2
        val subscribers = 5
        val mqttPublisher = arrayOfNulls<IMqttAsyncClient>(publishers)
        val mqttSubscriber = arrayOfNulls<IMqttAsyncClient>(subscribers)
        var connectToken: IMqttToken
        var subToken: IMqttToken
        var pubToken: IMqttDeliveryToken
        var disconnectToken: IMqttToken
        try {
            val topicNames = arrayOf("${nameRule.methodName}/Topic")
            val topicQos = intArrayOf(0)
            for (i in mqttPublisher.indices) {
                mqttPublisher[i] = MqttAndroidClient(InstrumentationRegistry.getInstrumentation().targetContext, mqttServerURI!!, "MultiPub$i")
                connectToken = mqttPublisher[i]!!.connect(null, null)
                Timber.i("publisher connecting url " + mqttServerURI + "MultiPub" + i)
                connectToken.waitForCompletion(waitForCompletionTime)
            } // for...
            val mqttReceiver = arrayOfNulls<MqttReceiver>(mqttSubscriber.size)
            for (i in mqttSubscriber.indices) {
                mqttSubscriber[i] =
                    MqttAndroidClient(InstrumentationRegistry.getInstrumentation().targetContext, mqttServerURI!!, "MultiSubscriber$i")
                mqttReceiver[i] = MqttReceiver(mqttSubscriber[i]!!)
                mqttSubscriber[i]!!.setCallback(mqttReceiver[i]!!)
                Timber.i("Assigning callback...")
                connectToken = mqttSubscriber[i]!!.connect(null, null)
                Timber.i("subscriber connecting url " + mqttServerURI + "MultiSubscriber" + i)
                connectToken.waitForCompletion(waitForCompletionTime)
                subToken = mqttSubscriber[i]!!.subscribe(topicNames, topicQos, null, null)
                Timber.i("subscribe " + topicNames[0] + " QoS is " + topicQos[0])
                subToken.waitForCompletion(waitForCompletionTime)
            }
            for (iMessage in 0..1) {
                val payload = "Message $iMessage".toByteArray()
                for (aMqttPublisher in mqttPublisher) {
                    pubToken = aMqttPublisher!!.publish(topicNames[0], payload, 0, false, null, null)
                    Timber.i("publish to " + topicNames[0] + " payload is " + payload.contentToString())
                    pubToken.waitForCompletion(waitForCompletionTime)
                }
                TimeUnit.MILLISECONDS.sleep(9999)
                for (i in mqttSubscriber.indices) {
                    for (aMqttPublisher in mqttPublisher) {
                        Timber.i("validate time = " + Date().toString())
                        val validateResult = mqttReceiver[i]!!.validateReceipt(topicNames[0], 0, payload)
                        if (!validateResult.ok) {
                            fail(validateResult.message)
                        }
                    }
                }
            }
        } catch (exception: Exception) {
            fail("Failed ${nameRule.methodName} exception=${exception.stackTraceToString()}")
        } finally {
            try {
                for (aMqttPublisher in mqttPublisher) {
                    disconnectToken = aMqttPublisher!!.disconnect(null, null)
                    disconnectToken.waitForCompletion(waitForCompletionTime)
                    aMqttPublisher.close()
                }
                for (aMqttSubscriber in mqttSubscriber) {
                    disconnectToken = aMqttSubscriber!!.disconnect(null, null)
                    disconnectToken.waitForCompletion(waitForCompletionTime)
                    aMqttSubscriber.close()
                }
            } catch (ignored: Exception) {
            }
        }
    }

    /**
     * Test that QOS values are preserved between MQTT publishers and subscribers.
     */
    @Test
    @Ignore("testQoSPreserved exception=Timed out waiting for a response from the server")
    fun testQoSPreserved() {
        var mqttClient: IMqttAsyncClient? = null
        val connectToken: IMqttToken
        val subToken: IMqttToken
        var pubToken: IMqttDeliveryToken
        val disconnectToken: IMqttToken
        try {
            mqttClient = MqttAndroidClient(InstrumentationRegistry.getInstrumentation().targetContext, mqttServerURI!!, "testQoSPreserved")
            val mqttReceiver = MqttReceiver(mqttClient)
            mqttClient.setCallback(mqttReceiver)
            connectToken = mqttClient.connect(null, null)
            connectToken.waitForCompletion(waitForCompletionTime)
            val topicNames = arrayOf("${nameRule.methodName}/Topic0", "${nameRule.methodName}/Topic1", "${nameRule.methodName}/Topic2")
            val topicQos = intArrayOf(0, 1, 2)
            subToken = mqttClient.subscribe(topicNames, topicQos, null, null)
            subToken.waitForCompletion(waitForCompletionTime)
            for (i in topicNames.indices) {
                val message = ("Message payload " + classSimpleName + ".${nameRule.methodName} " + topicNames[i]).toByteArray()
                for (iQos in 0..2) {
                    pubToken = mqttClient.publish(topicNames[i], message, iQos, false, null, null)
                    pubToken.waitForCompletion(waitForCompletionTime)
                    val validateResult = mqttReceiver.validateReceipt(topicNames[i], min(iQos, topicQos[i]), message)
                    if (!validateResult.ok) {
                        fail("${validateResult.message}\n\"Receive failed sub Qos=${topicQos[i]} PublishQos=$iQos")
                    }
                }
            }
        } catch (exception: Exception) {
            fail("Failed:${nameRule.methodName} exception=${exception.stackTraceToString()}")
        } finally {
            try {
                disconnectToken = mqttClient!!.disconnect(null, null)
                disconnectToken.waitForCompletion(waitForCompletionTime)
                mqttClient.close()
            } catch (ignored: Exception) {
            }
        }
    }

    @Test
    @Ignore("Trust anchor for certification path not found")
    fun testNonDurableSubs() {
        var mqttClient: IMqttAsyncClient? = null
        var connectToken: IMqttToken
        var subToken: IMqttToken
        val unSubToken: IMqttToken
        var pubToken: IMqttDeliveryToken
        var disconnectToken: IMqttToken
        try {
            mqttClient = MqttAndroidClient(InstrumentationRegistry.getInstrumentation().targetContext, mqttServerURI!!, "testNonDurableSubs")
            var mqttV3Receiver = MqttReceiver(mqttClient)
            mqttClient.setCallback(mqttV3Receiver)
            var mqttConnectOptions = MqttConnectOptions()
            // Clean session true is the default and implies non durable subscriptions.
            mqttConnectOptions.isCleanSession = true
            connectToken = mqttClient.connect(mqttConnectOptions, null, null)
            connectToken.waitForCompletion(10000)
            val topicNames = arrayOf("${nameRule.methodName}/Topic")
            val topicQos = intArrayOf(2)
            subToken = mqttClient.subscribe(topicNames, topicQos, null, null)
            subToken.waitForCompletion(10000)

            val payloadNotRetained = "Message payload $classSimpleName.${nameRule.methodName} not retained".toByteArray()
            pubToken = mqttClient.publish(topicNames[0], payloadNotRetained, 2, false, null, null)
            pubToken.waitForCompletion(100000)
            var validateResult = mqttV3Receiver.validateReceipt(topicNames[0], 2, payloadNotRetained)
            if (!validateResult.ok) {
                fail(validateResult.message)
            }

            // Retained publications.
            val payloadRetained = "Message payload $classSimpleName.${nameRule.methodName} retained".toByteArray()
            pubToken = mqttClient.publish(topicNames[0], payloadRetained, 2, true, null, null)
            pubToken.waitForCompletion(10000)
            validateResult = mqttV3Receiver.validateReceipt(topicNames[0], 2, payloadRetained)
            if (!validateResult.ok) {
                fail(validateResult.message)
            }

            // Check that unsubscribe and re subscribe resends the publication.
            unSubToken = mqttClient.unsubscribe(topicNames, null, null)
            unSubToken.waitForCompletion(10000)
            subToken = mqttClient.subscribe(topicNames, topicQos, null, null)
            subToken.waitForCompletion(10000)
            validateResult = mqttV3Receiver.validateReceipt(topicNames[0], 2, payloadRetained)
            if (!validateResult.ok) {
                fail(validateResult.message)
            }

            // Check that subscribe without unsubscribe receives the
            // publication.
            subToken = mqttClient.subscribe(topicNames, topicQos, null, null)
            subToken.waitForCompletion(10000)
            validateResult = mqttV3Receiver.validateReceipt(topicNames[0], 2, payloadRetained)
            if (!validateResult.ok) {
                fail(validateResult.message)
            }

            // Disconnect, reconnect and check that the retained publication is still delivered.
            disconnectToken = mqttClient.disconnect(InstrumentationRegistry.getInstrumentation().targetContext, this)
            disconnectToken.waitForCompletion(10000)
            mqttClient.close()
            mqttClient = MqttAndroidClient(InstrumentationRegistry.getInstrumentation().targetContext, mqttServerURI!!, "testNonDurableSubs")
            mqttV3Receiver = MqttReceiver(mqttClient)
            mqttClient.setCallback(mqttV3Receiver)
            mqttConnectOptions = MqttConnectOptions()
            mqttConnectOptions.isCleanSession = true
            connectToken = mqttClient.connect(mqttConnectOptions, null, null)
            connectToken.waitForCompletion(1000)
            subToken = mqttClient.subscribe(topicNames, topicQos, null, null)
            subToken.waitForCompletion(1000)
            validateResult = mqttV3Receiver.validateReceipt(topicNames[0], 2, payloadRetained)
            if (!validateResult.ok) {
                fail(validateResult.message)
            }
        } catch (exception: Exception) {
            fail("Failed:${nameRule.methodName} exception=${exception.stackTraceToString()}")
        } finally {
            try {
                disconnectToken = mqttClient!!.disconnect(null, null)
                disconnectToken.waitForCompletion(1000)
                mqttClient.close()
            } catch (ignored: Exception) {
            }
        }
    }

    /**
     * Test the behaviour of the cleanStart flag, used to clean up before
     * re-connecting.
     */
    @Test
    fun testCleanStart() {
        var mqttClient: IMqttAsyncClient? = null
        var connectToken: IMqttToken
        var subToken: IMqttToken
        var pubToken: IMqttDeliveryToken
        var disconnectToken: IMqttToken

        try {
            mqttClient = MqttAndroidClient(InstrumentationRegistry.getInstrumentation().targetContext, mqttServerURI!!, "testCleanStart")
            var mqttV3Receiver = MqttReceiver(mqttClient)
            mqttClient.setCallback(mqttV3Receiver)
            var mqttConnectOptions = MqttConnectOptions()
            // Clean start: true - The broker cleans up all client state,
            // including subscriptions, when the client is disconnected.
            // Clean start: false - The broker remembers all client state,
            // including subscriptions, when the client is disconnected.
            // Matching publications will get queued in the broker whilst the client is disconnected.
            // For Mqtt V3 cleanSession=false, implies new subscriptions are durable.
            mqttConnectOptions.isCleanSession = false
            connectToken = mqttClient.connect(mqttConnectOptions, null, null)
            connectToken.waitForCompletion(waitForCompletionTime)
            val topicNames = arrayOf("${nameRule.methodName}/Topic")
            val topicQos = intArrayOf(0)
            subToken = mqttClient.subscribe(topicNames, topicQos, null, null)
            subToken.waitForCompletion(waitForCompletionTime)
            var payload = "Message payload $classSimpleName.${nameRule.methodName} First".toByteArray()
            pubToken = mqttClient.publish(topicNames[0], payload, 1, false, null, null)
            pubToken.waitForCompletion(waitForCompletionTime)
            var validateResult = mqttV3Receiver.validateReceipt(topicNames[0], 0, payload)
            if (!validateResult.ok) {
                fail(validateResult.message)
            }

            // Disconnect and reconnect to make sure the subscription and all queued messages are cleared.
            disconnectToken = mqttClient.disconnect(InstrumentationRegistry.getInstrumentation().targetContext, this)
            disconnectToken.waitForCompletion(waitForCompletionTime)
            mqttClient.close()

            // Send a message from another client, to our durable subscription.
            mqttClient = MqttAndroidClient(InstrumentationRegistry.getInstrumentation().targetContext, mqttServerURI!!, "testCleanStart" + "Other")
            mqttV3Receiver = MqttReceiver(mqttClient)
            mqttClient.setCallback(mqttV3Receiver)
            mqttConnectOptions = MqttConnectOptions()
            mqttConnectOptions.isCleanSession = true
            connectToken = mqttClient.connect(mqttConnectOptions, null, null)
            connectToken.waitForCompletion(waitForCompletionTime)

            // Receive the publication so that we can be sure the first client has also received it.
            // Otherwise the first client may reconnect with its clean session before the message has arrived.
            subToken = mqttClient.subscribe(topicNames, topicQos, null, null)
            subToken.waitForCompletion(waitForCompletionTime)
            payload = "Message payload $classSimpleName.${nameRule.methodName} Other client".toByteArray()
            pubToken = mqttClient.publish(topicNames[0], payload, 1, false, null, null)
            pubToken.waitForCompletion(waitForCompletionTime)
            validateResult = mqttV3Receiver.validateReceipt(topicNames[0], 0, payload)
            if (!validateResult.ok) {
                fail(validateResult.message)
            }
            disconnectToken = mqttClient.disconnect(InstrumentationRegistry.getInstrumentation().targetContext, this)
            disconnectToken.waitForCompletion(waitForCompletionTime)
            mqttClient.close()

            // Reconnect and check we have no messages.
            mqttClient = MqttAndroidClient(InstrumentationRegistry.getInstrumentation().targetContext, mqttServerURI!!, "testCleanStart")
            mqttV3Receiver = MqttReceiver(mqttClient)
            mqttClient.setCallback(mqttV3Receiver)
            mqttConnectOptions = MqttConnectOptions()
            mqttConnectOptions.isCleanSession = true
            connectToken = mqttClient.connect(mqttConnectOptions, null, null)
            connectToken.waitForCompletion(waitForCompletionTime)
            var receivedMessage = mqttV3Receiver.receiveNext(100)
            if (receivedMessage != null) {
                fail("Receive message:" + String(receivedMessage.message.payload))
            }

            // Also check that subscription is cancelled.
            payload = "Message payload $classSimpleName.${nameRule.methodName} Cancelled Subscription".toByteArray()
            pubToken = mqttClient.publish(topicNames[0], payload, 1, false, null, null)
            pubToken.waitForCompletion(waitForCompletionTime)
            receivedMessage = mqttV3Receiver.receiveNext(100)
            if (receivedMessage != null) {
                fail("Receive message:" + String(receivedMessage.message.payload))
            }
        } catch (exception: Exception) {
            fail("Failed:${nameRule.methodName} exception=${exception.stackTraceToString()}")
        } finally {
            try {
                disconnectToken = mqttClient!!.disconnect(null, null)
                disconnectToken.waitForCompletion(waitForCompletionTime)
                mqttClient.close()
            } catch (ignored: Exception) {
            }
        }
    }

    @Test
    fun testPubSub() {
        var mqttClient: IMqttAsyncClient? = null
        try {
            mqttClient = MqttAndroidClient(InstrumentationRegistry.getInstrumentation().targetContext, mqttServerURI!!, nameRule.methodName)
            val subToken: IMqttToken
            val pubToken: IMqttDeliveryToken
            val mqttReceiver = MqttReceiver(mqttClient)
            mqttClient.setCallback(mqttReceiver)
            val connectToken: IMqttToken = mqttClient.connect(null, null)
            connectToken.waitForCompletion(waitForCompletionTime)
            val topicNames = arrayOf("testPubSub" + "/Topic")
            val topicQos = intArrayOf(0)
            val mqttMessage = MqttMessage("message for testPubSub".toByteArray())
            val message = mqttMessage.payload
            subToken = mqttClient.subscribe(topicNames, topicQos, null, null)
            subToken.waitForCompletion(waitForCompletionTime)
            pubToken = mqttClient.publish(topicNames[0], message, 0, false, null, null)
            pubToken.waitForCompletion(waitForCompletionTime)
            TimeUnit.MILLISECONDS.sleep(3000)
            val validateResult = mqttReceiver.validateReceipt(topicNames[0], 0, message)
            if (!validateResult.ok) {
                fail(validateResult.message)
            }
        } catch (exception: Exception) {
            fail("Failed to instantiate:${nameRule.methodName} exception=${exception.stackTraceToString()}")
        } finally {
            try {
                val disconnectToken: IMqttToken = mqttClient!!.disconnect(null, null)
                disconnectToken.waitForCompletion(waitForCompletionTime)
                mqttClient.close()
            } catch (ignored: Exception) {
            }
        }
    }

    //	/** Originally commented out from the fv test version
    //	 * Tests that invalid clientIds cannot connect.
    //	 *
    //	 * @throws Exception
    //	 */
    //	@Test
    //	public void testBadClientId() throws Exception {
    //		Log.banner(logger, class, methodName);
    //		logger.entering(classCanonicalName, methodName);
    //
    //		// Client ids with length errors are now trapped by the client
    //		// implementation.
    //		// String[] clientIds = new
    //		// String[]{"","Minus-ClientId","123456789012345678901234"};
    //		String[] clientIds = new String[] { "Minus-ClientId" };
    //		IMqttAsyncClient mqttClient = null;
    //		IMqttToken connectToken ;
    //		IMqttToken disconnectToken;
    //
    //		for (String clientId : clientIds) {
    //			try {
    //				mqttClient = new MqttAndroidClient(mContext, serverURI, "testConnect");
    //						clientId);
    //
    //				try {
    //					connectToken = mqttClient.connect(null, null);
    //					connectToken.waitForCompletion(1000);
    //					connectToken.reset();
    //
    //					disconnectToken = mqttClient.disconnect(null, null);
    //					disconnectToken.waitForCompletion(1000);
    //					disconnectToken.reset();
    //
    //					fail("We shouldn't have been able to connect!");
    //				} catch (MqttException exception) {
    //					// This is the expected exception.
    //					logger.fine("We expect an exception because we used an invalid client id");
    //					// logger.log(Level.SEVERE, "caught exception:", exception);
    //				}
    //			} catch (Exception exception) {
    //				logger.fine("Failed:" + methodName + " exception="
    //						+ exception.getClass().getName() + "."
    //						+ exception.getMessage());
    //				logger.exiting(classCanonicalName, methodName,
    //						new Object[] { exception });
    //				throw exception;
    //			}
    //		}
    //
    //		logger.exiting(classCanonicalName, methodName);
    //	}

    @Test
    fun testHAConnect() {
        var client: IMqttAsyncClient? = null
        try {
            try {
                val junk = "tcp://junk:123"
                client = MqttAndroidClient(InstrumentationRegistry.getInstrumentation().targetContext, junk, nameRule.methodName)
                val urls = arrayOf("tcp://junk", mqttServerURI)
                val options = MqttConnectOptions()
                options.serverURIs = urls
                Timber.i("HA connect")
                val connectToken = client.connect(options)
                connectToken.waitForCompletion(waitForCompletionTime)
                Timber.i("HA disconnect")
                val disconnectToken = client.disconnect(InstrumentationRegistry.getInstrumentation().targetContext, this)
                disconnectToken.waitForCompletion(waitForCompletionTime)
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
    fun testRetainedMessage() {
        val mqttClient: IMqttAsyncClient
        val mqttClientRetained: IMqttAsyncClient
        var disconnectToken: IMqttToken
        try {
            mqttClient = MqttAndroidClient(InstrumentationRegistry.getInstrumentation().targetContext, mqttServerURI!!, nameRule.methodName)
            var subToken: IMqttToken
            val pubToken: IMqttDeliveryToken
            val mqttReceiver = MqttReceiver(mqttClient)
            mqttClient.setCallback(mqttReceiver)
            var connectToken: IMqttToken = mqttClient.connect(null, null)
            connectToken.waitForCompletion(waitForCompletionTime)
            val topicNames = arrayOf("testRetainedMessage" + "/Topic")
            val topicQos = intArrayOf(0)
            val mqttMessage = MqttMessage("message for testPubSub".toByteArray())
            val message = mqttMessage.payload
            subToken = mqttClient.subscribe(topicNames, topicQos, null, null)
            subToken.waitForCompletion(waitForCompletionTime)
            pubToken = mqttClient.publish(topicNames[0], message, 0, true, null, null)
            pubToken.waitForCompletion(waitForCompletionTime)
            TimeUnit.MILLISECONDS.sleep(3000)
            var validateResult = mqttReceiver.validateReceipt(topicNames[0], 0, message)
            if (!validateResult.ok) {
                fail(validateResult.message)
            }
            Timber.i("First client received message successfully")
            disconnectToken = mqttClient.disconnect(InstrumentationRegistry.getInstrumentation().targetContext, this)
            disconnectToken.waitForCompletion(waitForCompletionTime)
            mqttClient.close()
            mqttClientRetained = MqttAndroidClient(InstrumentationRegistry.getInstrumentation().targetContext, mqttServerURI!!, "Retained")
            Timber.i("New MqttAndroidClient mqttClientRetained")
            val mqttV3ReceiverRetained = MqttReceiver(mqttClientRetained)
            mqttClientRetained.setCallback(mqttV3ReceiverRetained)
            Timber.i("Assigning callback...")
            connectToken = mqttClientRetained.connect(null, null)
            connectToken.waitForCompletion()
            Timber.i("Connect to mqtt server")
            subToken = mqttClientRetained.subscribe(topicNames, topicQos, null, null)
            subToken.waitForCompletion()
            Timber.i("subscribe " + topicNames[0] + " QoS is " + topicQos[0])
            TimeUnit.MILLISECONDS.sleep(3000)
            validateResult = mqttV3ReceiverRetained.validateReceipt(topicNames[0], 0, message)
            if (!validateResult.ok) {
                fail(validateResult.message)
            }
            Timber.i("Second client received message successfully")
            disconnectToken = mqttClientRetained.disconnect(InstrumentationRegistry.getInstrumentation().targetContext, this)
            disconnectToken.waitForCompletion(waitForCompletionTime)
            mqttClientRetained.close()
        } catch (exception: Exception) {
            fail("Failed ${nameRule.methodName} exception=${exception.stackTraceToString()}")
        }
    }

    /**
     * Tests that a client can be constructed and that it can connect to and
     * disconnect from the service via SSL
     */
    @Ignore("java.security.cert.CertPathValidatorException: Trust anchor for certification path not found")
    @Test
    fun testSSLConnect() {
        try {
            MqttAndroidClient(InstrumentationRegistry.getInstrumentation().targetContext, mqttSSLServerURI!!, "testSSLConnect").use { mqttClient ->
                val options = MqttConnectOptions()
                options.socketFactory =
                    mqttClient.getSSLSocketFactory(InstrumentationRegistry.getInstrumentation().targetContext.assets.open("test.bks"), keyStorePwd)
                var connectToken: IMqttToken = mqttClient.connect(options)
                connectToken.waitForCompletion(waitForCompletionTime)
                var disconnectToken: IMqttToken = mqttClient.disconnect(InstrumentationRegistry.getInstrumentation().targetContext, this)
                disconnectToken.waitForCompletion(waitForCompletionTime)
                connectToken = mqttClient.connect(options)
                connectToken.waitForCompletion(waitForCompletionTime)
                disconnectToken = mqttClient.disconnect(InstrumentationRegistry.getInstrumentation().targetContext, this)
                disconnectToken.waitForCompletion(waitForCompletionTime)
            }
        } catch (exception: Exception) {
            fail("Failed:testSSLConnect exception=${exception.stackTraceToString()}")
        }
    }

    /**
     * An SSL connection with server cert authentication, simple pub/sub of an message
     */
    @Test
    @Ignore("Trust anchor for certification path not found")
    fun testSSLPubSub() {
        var mqttClient: MqttAndroidClient? = null
        val connectToken: IMqttToken
        val disconnectToken: IMqttToken
        val subToken: IMqttToken
        val pubToken: IMqttDeliveryToken
        try {
            mqttClient = MqttAndroidClient(InstrumentationRegistry.getInstrumentation().targetContext, mqttSSLServerURI!!, "testSSLPubSub")
            val options = MqttConnectOptions()
            options.socketFactory =
                mqttClient.getSSLSocketFactory(InstrumentationRegistry.getInstrumentation().targetContext.assets.open("test.bks"), keyStorePwd)
            val mqttReceiver = MqttReceiver(mqttClient)
            mqttClient.setCallback(mqttReceiver)
            connectToken = mqttClient.connect(options)
            connectToken.waitForCompletion(waitForCompletionTime)
            val topicNames = arrayOf("testSSLPubSub" + "/Topic")
            val topicQos = intArrayOf(0)
            val mqttMessage = MqttMessage("message for testSSLPubSub".toByteArray())
            val message = mqttMessage.payload
            subToken = mqttClient.subscribe(topicNames, topicQos, null, null)
            subToken.waitForCompletion(waitForCompletionTime)
            pubToken = mqttClient.publish(topicNames[0], message, 0, false, null, null)
            pubToken.waitForCompletion(waitForCompletionTime)
            TimeUnit.MILLISECONDS.sleep(6000)
            val validateResult = mqttReceiver.validateReceipt(topicNames[0], 0, message)
            if (!validateResult.ok) {
                fail(validateResult.message)
            }
        } catch (exception: Exception) {
            fail("Failed:testSSLPubSub exception=${exception.stackTraceToString()}")
        } finally {
            disconnectToken = mqttClient!!.disconnect(InstrumentationRegistry.getInstrumentation().targetContext, this)
            disconnectToken.waitForCompletion(waitForCompletionTime)
            mqttClient.close()
        }
    }

    private inner class MqttConnectCallback : IMqttActionListener {
        var asyncActionToken: IMqttToken? = null
            private set

        override fun onSuccess(asyncActionToken: IMqttToken) {
            this.asyncActionToken = asyncActionToken
        }

        override fun onFailure(asyncActionToken: IMqttToken?, exception: Throwable?) = Unit
    }

    override fun onSuccess(asyncActionToken: IMqttToken) {
        Timber.d("onSuccess")
    }

    override fun onFailure(asyncActionToken: IMqttToken?, exception: Throwable?) {
        fail("onFailure")
    }

    companion object {
        private val classSimpleName = this::class.java.simpleName
        private val TAG = classSimpleName
    }
}