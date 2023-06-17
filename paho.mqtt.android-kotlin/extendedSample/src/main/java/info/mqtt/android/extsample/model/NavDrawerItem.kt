package info.mqtt.android.extsample.model

import info.mqtt.android.extsample.internal.Connection

class NavDrawerItem(connection: Connection) {
    val title: String = connection.id
    val handle: String = connection.handle()
}
