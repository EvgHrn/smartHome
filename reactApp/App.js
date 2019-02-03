import React from 'react';
import { StyleSheet, Text, View, Dimensions} from 'react-native';
import PouchDB from 'pouchdb-react-native';
import { DB_LOG, DB_PASS, DB_IP, DB_PORT } from 'react-native-dotenv';
import { LineChart } from 'react-native-chart-kit';

const lastDataCount = 300;

let remote_db = new PouchDB(`http://${DB_LOG}:${DB_PASS}@${DB_IP}:${DB_PORT}/smarthome`);

export default class App extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      data: [{
        timestamp:"",
        temperature: "",
        humidity: ""
      }]
    };
  }

  remoteChanges = remote_db.changes({
    since: 'now',
    live: true,
    include_docs: true
  }).
  on('change', () => {
    console.log('Remote db changed');
    this.getLastData();
  }).
  on('error', (err) => {
    console.log('Changes error with: ', remote_db);
    console.log(err);
  });

  getLastData = () => {
    remote_db.allDocs({
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
        let dateA = new Date(a.timestamp);
        let dateB = new Date(b.timestamp);
        return dateB - dateA;
      });
      //get just peace of data
      if (allDocs.length > lastDataCount) {
        allDocs.splice(lastDataCount);
      }
      this.setState(() => (
        {
          data: allDocs
        }
      ));
      console.log('State updated');
      // console.log(this.state.data);
    }.bind(this));
  }

  componentDidMount() {
    console.log('App did mount');
  }

  render() {
    const chartConfig = {
      backgroundGradientFrom: '#1E2923',
      backgroundGradientTo: '#08130D',
      color: (opacity = 1) => `rgba(26, 255, 146, ${opacity})`
    }
    return (
      <View style={styles.container}>
        <Text style={styles.header}>Date</Text>
        <Text>{this.state.data[0].timestamp}</Text>
        <Text style={styles.header}>Temperature</Text>
        <Text>{this.state.data[0].temperature}</Text>
        <Text style={styles.header}>Humidity</Text>
        <Text>{this.state.data[0].humidity}</Text>
        {/* <PureChart
          type='line'
          // data={ [ 50, 10, 40, 95, -4, -24, 85, 91, 35, 53, -53, 24, 50, -20, -80 ] }
          data={ this.state.data.map((item) => Math.round(item.temperature)) }
        >
        </PureChart > */}
        <LineChart
          // data={ {
          //   datasets: [{
          //     temp: this.state.data.map((item) => Math.round(item.temperature)),
          //     // hum: this.state.data.map((item) => Math.round(item.humidity))
          //   }]
          // }}
          data={{
            labels: ['January', 'February', 'March', 'April', 'May', 'June'],
            datasets: [{
              data: [
                Math.random() * 100,
                Math.random() * 100,
                Math.random() * 100,
                Math.random() * 100,
                Math.random() * 100,
                Math.random() * 100
              ]
            }]
          }}
          width={Dimensions.get('window').width}
          height={220}
          chartConfig={chartConfig}
          bezier
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
  },
  header: {
    color: 'gray'
  }
});
