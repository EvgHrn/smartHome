import React from 'react';
import { StyleSheet, Text, View } from 'react-native';
import PouchDB from 'pouchdb';

let remote_db = new PouchDB(`http://${process.env.DB_LOG}:${process.env.DB_PASS}@${process.env.DB_IP}:${process.env.DB_PORT}/smarthome`);
let local_db = new PouchDB('smarthome');

export default class App extends React.Component {
  render() {
    return (
      <View style={styles.container}>
        <Text>Date</Text>
        <Text>Temperature</Text>
        <Text>Humidity</Text>
      </View>
    );
  }
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#fff',
    alignItems: 'center',
    justifyContent: 'center',
  },
});
