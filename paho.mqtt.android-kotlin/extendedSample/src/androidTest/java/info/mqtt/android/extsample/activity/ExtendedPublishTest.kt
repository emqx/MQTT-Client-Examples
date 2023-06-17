package info.mqtt.android.extsample.activity

import android.view.Gravity
import androidx.test.core.graphics.writeToTestStorage
import androidx.test.espresso.Espresso.onView
import androidx.test.espresso.action.ViewActions.*
import androidx.test.espresso.assertion.ViewAssertions.matches
import androidx.test.espresso.contrib.DrawerActions
import androidx.test.espresso.contrib.DrawerMatchers.isClosed
import androidx.test.espresso.matcher.ViewMatchers
import androidx.test.espresso.matcher.ViewMatchers.isChecked
import androidx.test.espresso.matcher.ViewMatchers.withId
import androidx.test.espresso.matcher.ViewMatchers.withText
import androidx.test.espresso.screenshot.captureToBitmap
import androidx.test.ext.junit.rules.activityScenarioRule
import androidx.test.ext.junit.runners.AndroidJUnit4
import com.moka.lib.assertions.WaitingAssertion
import info.hannes.timber.DebugFormatTree
import info.mqtt.android.extsample.MainActivity
import info.mqtt.android.extsample.R
import org.hamcrest.CoreMatchers.not
import org.junit.Before
import org.junit.Rule
import org.junit.Test
import org.junit.rules.TestName
import org.junit.runner.RunWith
import timber.log.Timber


@RunWith(AndroidJUnit4::class)
class ExtendedPublishTest {

    // a handy JUnit rule that stores the method name, so it can be used to generate unique screenshot files per test method
    @get:Rule
    var nameRule = TestName()

    @get:Rule
    val activityScenarioRule = activityScenarioRule<MainActivity>()

    @Before
    fun setUp() {
        Timber.plant(DebugFormatTree())
    }

    @Test
    fun connectAndPublish() {
        onView(withId(R.id.drawer_layout))
            .check(matches(isClosed(Gravity.LEFT))) // Left Drawer should be closed.
            .perform(DrawerActions.open())
        onView(withId(R.id.action_add_connection)).perform(click())
        onView(withId(R.id.action_save_connection)).perform(click())

        onView(ViewMatchers.isRoot())
            .captureToBitmap()
            .writeToTestStorage("${javaClass.simpleName}_${nameRule.methodName}-AddConnect")

        onView(withId(R.id.disConnectSwitch)).perform(click())
        onView(withId(3)).perform(click())
        //onView(withTagValue(`is`("Subscribe" as Any))).perform(click())

        onView(withId(R.id.subscribe_button)).perform(click())
        onView(withId(R.id.subscription_topic_edit_text)).perform(typeText(TOPIC))
        onView(ViewMatchers.isRoot())
            .captureToBitmap()
            .writeToTestStorage("${javaClass.simpleName}_${nameRule.methodName}-Subscribe")
        onView(withText("OK")).perform(click())

        onView(withId(2)).perform(click())
        onView(withId(R.id.topic)).perform(replaceText(TOPIC))
        onView(withId(R.id.message)).perform(replaceText("msg"))
        onView(ViewMatchers.isRoot())
            .captureToBitmap()
            .writeToTestStorage("${javaClass.simpleName}_${nameRule.methodName}-publish")
        onView(withId(R.id.publish_button)).perform(click())

        onView(withId(1)).perform(click())

        WaitingAssertion.checkAssertion(R.id.history_list_view, Matchers.withListSizeBigger(0), 2500)
        onView(ViewMatchers.isRoot())
            .captureToBitmap()
            .writeToTestStorage("${javaClass.simpleName}_${nameRule.methodName}-End")
    }

    @Test
    fun disconnect() {
        onView(ViewMatchers.isRoot())
            .captureToBitmap()
            .writeToTestStorage("${javaClass.simpleName}_${nameRule.methodName}-Before")
        // it should be checked on previous test
        onView(withId(R.id.disConnectSwitch)).check(matches(isChecked()))

        onView(withId(R.id.disConnectSwitch)).perform(click())
        onView(ViewMatchers.isRoot())
            .captureToBitmap()
            .writeToTestStorage("${javaClass.simpleName}_${nameRule.methodName}-isDisConnected")

        onView(withId(R.id.disConnectSwitch)).check(matches(not(isChecked())))

        onView(withId(R.id.disConnectSwitch)).perform(click())
        onView(ViewMatchers.isRoot())
            .captureToBitmap()
            .writeToTestStorage("${javaClass.simpleName}_${nameRule.methodName}-isConnectedAgain")

        onView(withId(R.id.disConnectSwitch)).check(matches(isChecked()))
    }
    companion object {
        private const val TOPIC = "AnotherTest"
    }
}
