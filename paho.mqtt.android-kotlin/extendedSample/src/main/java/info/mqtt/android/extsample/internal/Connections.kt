package info.mqtt.android.extsample.internal

import android.content.Context
import info.mqtt.android.extsample.room.AppDatabase
import info.mqtt.android.extsample.room.PersistenceDao
import info.mqtt.android.extsample.utils.toConnection
import info.mqtt.android.extsample.utils.toConnectionEntity
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.async
import kotlinx.coroutines.launch
import kotlinx.coroutines.runBlocking
import timber.log.Timber


class Connections private constructor(context: Context) {

    var connections: HashMap<String, Connection> = HashMap()

    private var persistence: PersistenceDao = AppDatabase.getDatabase(context).persistenceDao()

    init {
        runBlocking {
            readConnectionsAsync(context).await()
        }
    }

    private fun readConnectionsAsync(context: Context) = CoroutineScope(Dispatchers.IO).async {
        synchronized(connections) {
            val connectionDaoList = persistence.all.map { it.toConnection(context) }

            connectionDaoList.forEach {
                Timber.d("Connection was persisted. ${it.handle()}")
                connections[it.handle()] = it
            }
        }
    }

    fun getConnection(handle: String): Connection? {
        synchronized(connections) {
            return connections[handle]
        }
    }

    fun addConnection(connection: Connection) {
        synchronized(connections) {
            connections[connection.handle()] = connection
        }
        CoroutineScope(Dispatchers.IO).launch {
            persistence.insert(connection.toConnectionEntity())
        }
    }

    fun removeConnection(connection: Connection) {
        synchronized(connections) {
            connections.remove(connection.handle())
        }
        CoroutineScope(Dispatchers.IO).launch {
            persistence.delete(connection.toConnectionEntity())
        }
    }

    fun updateConnection(connection: Connection) {
        synchronized(connections) {
            connections[connection.handle()] = connection
        }
        CoroutineScope(Dispatchers.IO).launch {
            persistence.updateAll(connection.toConnectionEntity())
        }
    }

    companion object {
        private var instance: Connections? = null

        @Synchronized
        fun getInstance(context: Context): Connections {
            if (instance == null) {
                instance = Connections(context)
            }
            return instance!!
        }
    }
}
