# QT MQTT client examples 

## Qt install

- All platform Qt5.7 can find [here](https://download.qt.io/new_archive/qt/5.7/5.7.0/). if your platform is ubuntu20.04, you can download qt downloader by run:

  ```shell
  wget https://download.qt.io/new_archive/qt/5.7/5.7.0/qt-opensource-linux-x64-5.7.0.run
  ```

- then, we change the execution authority of qt-opensource-linux-x64-5.7.0.run and execute.

  ```shell
  sudo chmod u+x qt-opensource-linux-x64-5.7.0.run
  ./qt-opensource-linux-x64-5.7.0.run
  ```

- find your qmake installed path, set qmake run path.

  ```shell
  PATH=$PATH:$PWD
  export PATH
  ```

## Get qmqtt 

```shell
git clone https://github.com/emqx/qmqtt.git
```

## Compile qmqtt

```shell
cd qmqtt
qmake
make
sudo make install
```

## Get sample code

```shell
git clone https://github.com/emqx/MQTT-Client-Examples.git
```

## Compile sample

```shell
cd MQTT-Client-Examples/mqtt-client-Qt/
mkdir build
cd build
qmake ..
make
./client/qmqtt_example
```

when you run qmqtt_example, make sure a local broker can access.