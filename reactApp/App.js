import React from 'react';
import { StyleSheet, Text, View, Dimensions, TouchableWithoutFeedback} from 'react-native';
import PouchDB from 'pouchdb-react-native';
import { DB_LOG, DB_PASS, DB_IP, DB_PORT } from 'react-native-dotenv';
import { SegmentedControls } from 'react-native-radio-buttons';
import moment from 'moment';
import { Charts } from './components/Charts';

let remote_db = new PouchDB(`http://${DB_LOG}:${DB_PASS}@${DB_IP}:${DB_PORT}/smarthome`);
let local_db = new PouchDB('smarthome');

export default class App extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      roughData: [{
        timestamp:"Mon, 04 Feb 2019 10:48:53 GMT",
        temperature: 0,
        humidity: 0
      }],
      period: "Day",
      progress: false
    };
  }

  rep = PouchDB.replicate(remote_db, local_db, {
    live: true,
    retry: true,
    batch_size: 5000
  }).on('change', function (info) {
    // handle change
    console.log('Remote db changed');
  }).on('paused', function (err) {
    // replication paused (e.g. replication up to date, user went offline)
    console.log('Replication paused: ', err);
  }).on('active', function () {
    // replicate resumed (e.g. new changes replicating, user went back online)
    console.log('Replication active');
  }).on('denied', function (err) {
    // a document failed to replicate (e.g. due to permissions)
    console.log('Replication denied with error: ', err);
  }).on('complete', function (info) {
    // handle complete
    console.log('Replication complete');
  }).on('error', function (err) {
    // handle error
    console.log('Replication error: ', err);
  });

  localChanges = local_db.changes({
    since: 'now',
    live: true,
    include_docs: true
  }).on('change', (change) => {
    // change.id contains the doc id, change.doc contains the doc
    console.log('Local db changed');
    if (change.deleted) {
      // document was deleted
      console.log('Something was deleted from db');
      return;
    } else {
      // document was added/modified
      console.log('New doc was added to loacldb: ', change.doc);
      let newRoughData = this.state.roughData;
      newRoughData.push({
        timestamp: change.doc.timestamp,
        temperature: change.doc.temperature,
        humidity: change.doc.humidity
      });
      this.setState({
        roughData: newRoughData
      });
    }
  }).on('error', function (err) {
    // handle errors
    console.log('Error while change local db: ', err);
  });

  localDbToState() {
    this.getLastDataToState(local_db);
  }

  mergeDataByPeriod() {
    console.log('Start to merge data with period ');
    const period = this.state.period;
    console.log('Period from state: ', period);
    //Collect data with periods
    let sourceDataObj;
    switch (period) {
      case "Day": 
        console.log('Day');
        sourceDataObj = this.state.roughData.reduce((accumulator, currentValue) => {
          let currTimeMomentObj = moment(currentValue.timestamp);
          let startOfHourStr = currTimeMomentObj.startOf('hour').toDate().toString();
          if (startOfHourStr in accumulator) {
            accumulator[startOfHourStr].temps.push(currentValue.temperature);
            accumulator[startOfHourStr].hums.push(currentValue.humidity);
          } else {
            accumulator[startOfHourStr] = {
              temps: [currentValue.temperature],
              hums: [currentValue.humidity]
            };
          };
          return accumulator;
        }, {});
        break;
      case "Week": 
      case "Month": 
        console.log('Week or Month');
        sourceDataObj = this.state.roughData.reduce((accumulator, currentValue, index, array) => {
          let currTimeMomentObj = moment(currentValue.timestamp);
          let startOfDayStr = currTimeMomentObj.startOf('day').toDate().toString();
          if (startOfDayStr in accumulator) {
            accumulator[startOfDayStr].temps.push(currentValue.temperature);
            accumulator[startOfDayStr].hums.push(currentValue.humidity);
          } else {
            accumulator[startOfDayStr] = {
              temps: [currentValue.temperature],
              hums: [currentValue.humidity]
            }
          };
          return accumulator;
        }, {});
      break;
      case "Hour": 
        console.log('Hour');
        sourceDataObj = this.state.roughData.reduce((accumulator, currentValue, index, array) => {
          let currTimeMomentObj = moment(currentValue.timestamp);
          let startOfMinuteStr = currTimeMomentObj.startOf('minute').toDate().toString();
          if (startOfMinuteStr in accumulator) {
            accumulator[startOfMinuteStr].temps.push(currentValue.temperature);
            accumulator[startOfMinuteStr].hums.push(currentValue.humidity);
          } else {
            accumulator[startOfMinuteStr] = {
              temps: [currentValue.temperature],
              hums: [currentValue.humidity]
            }
          };
          return accumulator;
        }, {});
        break;
      default:
      break;
    }

    //Average collected data and make data array
    let dataArr = [];
    for (var key in sourceDataObj) {
      let tempsArr = sourceDataObj[key].temps;
      let humsArr = sourceDataObj[key].hums;
      let averageTemp = tempsArr.reduce((acc, curr) => acc + parseFloat(curr), 0.0)/tempsArr.length;
      let averageHum = humsArr.reduce((acc, curr) => acc + parseFloat(curr), 0.0)/humsArr.length;
      dataArr.push({
        timestamp: key,
        temperature: averageTemp.toFixed(2),
        humidity: averageHum.toFixed(2)
      });
    }

    //sort by timestamp desc
    dataArr.sort((a, b) => {
      let dateA = new Date(a.timestamp);
      let dateB = new Date(b.timestamp);
      return dateB - dateA;
    });

    //get just peace of data
    let lastDataCount;
    switch (this.state.period) {
      case "Hour":
        lastDataCount = 60;
        break;
      case "Day":
        lastDataCount = 24;
        break;
      case "Week":
        lastDataCount = 7;
        break;
      case "Month":
        lastDataCount = 30;
        break;
      default:
        break;
    }
    if (dataArr.length > lastDataCount) {
      dataArr.splice(lastDataCount);
    }
    console.log('Finish to merge rough data to renderdata');
    return dataArr;
  }

  getRoughLocalDataToState() {
    local_db.allDocs({
      include_docs: true,
      attachments: false
    }, (err, response) => {
      if (err) { return console.log(err); }
      // handle result
      const allDocs = response.rows.map((currentValue, index, array) => {
        let resultObj = {
          timestamp: currentValue.doc.timestamp,
          temperature: currentValue.doc.temperature,
          humidity: currentValue.doc.humidity,
        };
        return resultObj;
      });
      this.setState({
        roughData: allDocs
      });
    });
  }

  componentDidMount() {
    console.log('App did mount and state update with local_db data');
    this.getRoughLocalDataToState();
  }

  render() {
    const renderData = this.mergeDataByPeriod();
    console.log('Length of new renderdata : ', renderData.length)
    console.log('Render');
    const radioBtnsOptions = [
      "Hour",
      "Day",
      "Week",
      "Month"
    ];

    setSelectedOption = (selectedOption) => {
      this.setState({
        period: selectedOption
      });
      console.log("Radiobutton pressed: ", selectedOption);
    }
  
    renderOption = (option, selected, onSelect, index) => {
      const style = selected ? { fontWeight: 'bold'} : {};
  
      return (
        <TouchableWithoutFeedback onPress={onSelect} key={index}>
          <Text style={style}>{option}</Text>
        </TouchableWithoutFeedback>
      );
    }
  
    renderContainer = (optionNodes) => {
      return <View>{optionNodes}</View>;
    }

    return (
      <View style={styles.container}>
        <Text style={[styles.header, styles.row]}>Date</Text>
        <Text>{new Date(renderData[0].timestamp).toLocaleString()}</Text>
        <Text style={[styles.header, styles.row]}>Temperature</Text>
        <Text>{renderData[0].temperature}</Text>
        <Text style={[styles.header, styles.row]}>Humidity</Text>
        <Text>{renderData[0].humidity}</Text>
        <Charts renderData={renderData}/>
        <SegmentedControls
          options={ radioBtnsOptions }
          onSelection={ setSelectedOption.bind(this) }
          selectedOption={ this.state.period }
        />
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
    paddingVertical: 20
  },
  header: {
    color: 'gray'
  },
  row: {
    paddingVertical: 10
  }
});
