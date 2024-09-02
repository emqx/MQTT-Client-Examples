# EMQX React Native (Expo) MQTT Demo

This is an [Expo](https://expo.dev) project created with [`create-expo-app`](https://www.npmjs.com/package/create-expo-app).
This demo is just for you to learn how to connect to [EMQX](https://www.emqx.com) in React Native using [MQTT.js](https://github.com/mqttjs/MQTT.js).

## Prerequisites

1. Install Android Studio or Xcode for Android emulators and iOS simulators. Refer to the [React Native Doc](https://reactnative.dev/docs/set-up-your-environment) for more details.
2. Install Expo Go on your mobile phone. Refer to the [Expo Doc](https://docs.expo.dev/get-started/set-up-your-environment/) for more details.
3. Install Node.js v18.0 or higher.

## Get started

1. Install dependencies

   ```bash
   npm install
   ```

2. Start the app

   ```bash
    npx expo start
   ```

In the output, you'll find options to open the app in a

- [development build](https://docs.expo.dev/develop/development-builds/introduction/)
- [Android emulator](https://docs.expo.dev/workflow/android-studio-emulator/)
- [iOS simulator](https://docs.expo.dev/workflow/ios-simulator/)
- [Expo Go](https://expo.dev/go), a limited sandbox for trying out app development with Expo

You can start developing by editing the files inside the **app** directory. This project uses [file-based routing](https://docs.expo.dev/router/introduction).

## Note

To use the `mqtt` package, follow these steps:

1. Update Babel configuration:

   ```javascript
   plugins: ["react-native-reanimated/plugin"],
   ```

2. Install the `url` package:

   ```bash
   npm install url
   ```
