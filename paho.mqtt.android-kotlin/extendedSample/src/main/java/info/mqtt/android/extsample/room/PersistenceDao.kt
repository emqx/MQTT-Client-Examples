package info.mqtt.android.extsample.room

import androidx.room.*
import info.mqtt.android.extsample.room.entity.ConnectionEntity

@Dao
interface PersistenceDao {

    @get:Query("SELECT * FROM ConnectionEntity")
    val all: List<ConnectionEntity>

    @Insert(onConflict = OnConflictStrategy.REPLACE)
    fun insert(connectionEntity: ConnectionEntity): Long

    @Update
    fun updateAll(vararg connectionEntities: ConnectionEntity)

    @Delete
    fun delete(connectionEntity: ConnectionEntity)
}
