package info.mqtt.android.service.room

import androidx.room.*
import info.mqtt.android.service.room.entity.MqMessageEntity

@Dao
interface MqMessageDao {

    @get:Query("SELECT * FROM MQMessageEntity")
    val all: List<MqMessageEntity>

    @Query("SELECT * FROM MQMessageEntity WHERE clientHandle = :clientHandle ORDER BY timestamp ASC")
    fun allArrived(clientHandle: String): List<MqMessageEntity>

    @Insert(onConflict = OnConflictStrategy.REPLACE)
    fun insert(MqMessageEntity: MqMessageEntity): Long

    @Update
    fun updateAll(vararg MqMessageEntities: MqMessageEntity)

    @Delete
    fun delete(MqMessageEntity: MqMessageEntity)

    @Query("DELETE FROM MqMessageEntity WHERE clientHandle = :clientHandle AND messageId = :id")
    fun deleteId(clientHandle: String, id: String): Int

    @Query("DELETE FROM MqMessageEntity WHERE clientHandle = :clientHandle")
    fun deleteClientHandle(clientHandle: String): Int

}
