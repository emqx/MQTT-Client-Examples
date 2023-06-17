package info.mqtt.android.extsample.room

import android.content.Context
import androidx.room.Database
import androidx.room.Room
import androidx.room.RoomDatabase
import androidx.room.TypeConverters
import info.mqtt.android.extsample.room.AppDatabase.Companion.DB_VERSION

import info.mqtt.android.extsample.room.entity.ConnectionEntity
import info.mqtt.android.extsample.room.entity.SubscriptionEntity
import info.mqtt.android.service.room.Converters

@Database(entities = [ConnectionEntity::class, SubscriptionEntity::class], version = DB_VERSION)
@TypeConverters(Converters::class)
abstract class AppDatabase : RoomDatabase() {

    abstract fun persistenceDao(): PersistenceDao

    abstract fun subscriptionDao(): SubscriptionDao

    @Suppress("SimpleRedundantLet")
    companion object {

        const val DB_VERSION = 1
        private var db: AppDatabase? = null
        const val DB_NAME = "persistenceMQ"

        @Synchronized
        fun getDatabase(context: Context, storageName: String = DB_NAME): AppDatabase {
            return db?.let {
                it
            } ?: run {
                db = Room.databaseBuilder(
                    context.applicationContext,
                    AppDatabase::class.java,
                    storageName
                ).build()
                db!!
            }
        }
    }
}
