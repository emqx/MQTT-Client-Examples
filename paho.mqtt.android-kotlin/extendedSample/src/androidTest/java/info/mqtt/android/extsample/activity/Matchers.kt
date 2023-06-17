package info.mqtt.android.extsample.activity

import android.view.View
import android.widget.ListView
import org.hamcrest.Description
import org.hamcrest.Matcher
import org.hamcrest.TypeSafeMatcher

internal object Matchers {
    fun withListSize(size: Int): Matcher<View> {
        return object : TypeSafeMatcher<View>() {
            public override fun matchesSafely(view: View): Boolean {
                return (view as ListView).count == size
            }

            override fun describeTo(description: Description) {
                description.appendText("ListView should have $size items")
            }
        }
    }

    fun withListSizeBigger(size: Int): Matcher<View> {
        return object : TypeSafeMatcher<View>() {
            public override fun matchesSafely(view: View): Boolean {
                return (view as ListView).count > size
            }

            override fun describeTo(description: Description) {
                description.appendText("ListView should have > $size items")
            }
        }
    }
}
