import React from 'react';
import { StyleSheet, View, Dimensions} from 'react-native';
import { LineChart } from 'react-native-chart-kit';

export class Charts extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
        };
    }
    render() {
        const renderData = this.props.renderData;
        return(
            <View>
                <LineChart
                    data={{
                        // labels: this.state.data.map((item) => new Date(item.timestamp).getHours()),
                        datasets: [{
                            data: renderData.map((item) => parseFloat(item.temperature))
                        }]
                    }}
                    chartConfig = {{
                        decimalPlaces: 2,
                        backgroundGradientFrom: '#FFFFFF',
                        backgroundGradientTo: '#FFFFFF',
                        color: (opacity = 1) => `rgba(150, 70, 70, ${opacity})`
                    }}
                    width={Dimensions.get('window').width}
                    height={120}
                    bezier
                    style={{
                        marginVertical: 15
                    }}
                />
                <LineChart
                    data={{
                    // labels: this.state.data.map((item) => parseFloat(new Date(item.timestamp).getHours())),
                        datasets: [{
                            data: renderData.map((item) => parseFloat(item.humidity))
                        }]
                    }}
                    chartConfig = {{
                        decimalPlaces: 2,
                        backgroundGradientFrom: '#FFFFFF',
                        backgroundGradientTo: '#FFFFFF',
                        color: (opacity = 1) => `rgba(70, 70, 150, ${opacity})`
                    }}
                    width={Dimensions.get('window').width}
                    height={120}
                    bezier
                    style={{
                        marginVertical: 15
                    }}
                />
            </View>
        );
    }
}
