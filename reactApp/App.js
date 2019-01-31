import React from 'react';
import { StyleSheet, Text, View } from 'react-native';
import PouchDB from 'pouchdb';

require('dotenv').config();

let remote_db = new PouchDB(`http://${process.env.DB_LOG}:${process.env.DB_PASS}@${process.env.DB_IP}:${process.env.DB_PORT}/smarthome`);
let local_db = new PouchDB('smarthomeDb');

export default class App extends React.Component {
  getDerivedStateFromProps(props, state) {
    var sync = PouchDB.sync(local_db, remote_db, {
      live: true,
      retry: true
    }).on('change', function (info) {
      // handle change
    }).on('paused', function (err) {
      // replication paused (e.g. replication up to date, user went offline)
    }).on('active', function () {
      // replicate resumed (e.g. new changes replicating, user went back online)
    }).on('denied', function (err) {
      // a document failed to replicate (e.g. due to permissions)
    }).on('complete', function (info) {
      // handle complete
    }).on('error', function (err) {
      // handle error
    });
  }
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
