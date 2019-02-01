import React from 'react';
import { StyleSheet, Text, View } from 'react-native';
import PouchDB from 'pouchdb';
PouchDB.plugin(require('pouchdb-find'));
import { DB_LOG, DB_PASS, DB_IP, DB_PORT } from 'react-native-dotenv';

const lastDataCount = 300;

let remote_db = new PouchDB(`http://${DB_LOG}:${DB_PASS}@${DB_IP}:${DB_PORT}/smarthome`);
let local_db = new PouchDB('smarthomeDb');

let getLastData = () => {
  local_db.allDocs({
    include_docs: true,
    attachments: false
  }, function(err, response) {
    if (err) { return console.log(err); }
    // handle result
    let allDocs = response.rows.map((currentValue, index, array) => {
      let resultObj = {
        timestamp: currentValue.doc.timestamp,
        temperature: currentValue.doc.temperature,
        humidity: currentValue.doc.humidity,
      };
      return resultObj;
    });
    //lets sort by timestamp desc
    allDocs.sort((a, b) => {
      let dateA = new Date(a.date);
      let dateB = new Date(b.date);
      return dateB - dateA;
    });
    //get just peace of data
    if (allDocs.length > lastDataCount) {
      allDocs.splice(lastDataCount);
    }
    return allDocs;
  });
}

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
