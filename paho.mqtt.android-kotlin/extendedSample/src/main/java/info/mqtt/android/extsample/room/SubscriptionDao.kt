package info.mqtt.android.extsample.room

import androidx.room.*
import info.mqtt.android.extsample.room.entity.SubscriptionEntity

@Dao
interface SubscriptionDao {

    @get:Query("SELECT * FROM SubscriptionEntity")
    val all: List<SubscriptionEntity>

    @Insert(onConflict = OnConflictStrategy.REPLACE)
    fun insert(subscriptionEntity: SubscriptionEntity): Long

    @Update
    fun updateAll(vararg entities: SubscriptionEntity)

    @Delete
    fun delete(subscriptionEntity: SubscriptionEntity)
}
