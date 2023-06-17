package info.mqtt.android.extsample.utils

import info.mqtt.android.extsample.model.Subscription
import info.mqtt.android.extsample.room.entity.SubscriptionEntity

fun SubscriptionEntity.toSubscription(): Subscription = Subscription(
    this.topic,
    this.qos,
    this.clientHandle,
    this.notify.toBoolean()
)

fun Subscription.toSubscriptionEntity(): SubscriptionEntity = SubscriptionEntity(
    this.clientHandle,
    this.topic,
    this.isEnableNotifications.toInt(),
    this.qos
)
