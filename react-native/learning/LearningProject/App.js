import React, { Component } from 'react';
import {
	StyleSheet,
	Text,
	View,
	Image,
	TextInput,
	Button,
	ScrollView,
	FlatList,
	SectionList,
	ActivityIndicator
} from 'react-native';

export class Greeting extends Component {
	render() {
		return <Text>Hello, {this.props.name}</Text>;
	}
}

export class Blink extends Component {
	// state object
	state = { isShowingText: true };

	componentDidMount() {
		setInterval(() => this.setState((previousState) => ({ isShowingText: !previousState.isShowingText })), 1000);
	}

	render() {
		if (!this.state.isShowingText) {
			return null;
		}

		return <Text style={[ styles.smallText, styles.redText ]}>{this.props.text}</Text>;
	}
}

export class PizzaTranslator extends Component {
	constructor(props) {
		super(props);
		this.state = { text: '' };
	}

	render() {
		return (
			<View style={{ padding: 10 }}>
				<TextInput
					style={{ height: 40 }}
					placeholder="Type to translate"
					onChangeText={(text) => this.setState({ text })}
					value={this.state.text}
				/>
				<Text style={{ padding: 10, fontSize: 42 }}>
					{this.state.text.split(' ').map((word) => word && '🍕').join(' ')}
				</Text>
			</View>
		);
	}
}

export class ButtonBasics extends Component {
	_onPressButton() {
		alert('You tapped the button!');
	}

	render() {
		return (
			<View>
				<Button onPress={this._onPressButton} title="Press me" />
			</View>
		);
	}
}

let data = [
	{ title: 'D', data: [ 'Devin', 'Dan', 'Dominic' ] },
	{ title: 'J', data: [ 'Jackson', 'James', 'Jillian', 'Jimmy', 'Joel', 'John', 'Joseph' ] },
	{ title: 'M', data: [ 'Michael', 'Mao', 'Marx' ] }
];

export class SectionListBasics extends Component {
	render() {
		return (
			<SectionList
				sections={data}
				renderItem={({ item }) => <Text style={styles.item}>{item}</Text>}
				renderSectionHeader={({ section }) => <Text style={styles.sectionHeader}>{section.title}</Text>}
				keyExtractor={(item, index) => index}
			/>
		);
	}
}

export class FetchExample extends Component {
	constructor(props) {
		super(props);
		this.state = { isLoading: true };
	}

	componentDidMount() {
		return fetch('https://facebook.github.io/react-native/movies.json')
			.then((response) => response.json())
			.then((responseJson) => {
				this.setState({
					isLoading: false,
					dataSource: responseJson.movies
				});
			})
			.catch((error) => {
				console.error(error);
			});
	}

	render() {
		if (this.state.isLoading) {
			return (
				<View style={{ flex: 1, padding: 20 }}>
					<ActivityIndicator />
				</View>
			);
		}

		return (
			<View style={{ flex: 1, paddingtop: 20 }}>
				<FlatList
					data={this.state.dataSource}
					renderItem={({ item }) => <Text>{item.title}</Text>}
					keyExtractor={({ id }, index) => id}
				/>
			</View>
		);
	}
}

export default function App() {
	return (
		<View style={[ styles.container, styles.blueBackground ]}>
			<View style={{ flex: 1, backgroundColor: 'white' }}>
				<Text style={{ fontSize: 50 }}>Header</Text>
			</View>
			<View style={[ { flex: 5 }, styles.blueBackground ]}>
				<ScrollView>
					<Text>Open up App.js to start working on your app!</Text>
					<Image style={[ styles.icon, styles.blueBackground ]} source={require('./assets/icon.png')} />
					<Image style={{ height: 100, width: 100 }} source={require('./assets/background.jpg')} />
					<Image style={{ height: 100, width: 100 }} source={require('./assets/background.jpg')} />
					<Image style={{ height: 100, width: 100 }} source={require('./assets/background.jpg')} />
					<Image style={{ height: 100, width: 100 }} source={require('./assets/background.jpg')} />
					<Image style={{ height: 100, width: 100 }} source={require('./assets/background.jpg')} />
					<Image style={{ height: 100, width: 100 }} source={require('./assets/background.jpg')} />
					<Image style={{ height: 100, width: 100 }} source={require('./assets/background.jpg')} />
					<Greeting name="Michael" />
					<Blink text="Blinking text" />
					<PizzaTranslator />
					<ButtonBasics />
					<SectionListBasics />
					<FetchExample />
				</ScrollView>
			</View>
		</View>
	);
}

const styles = StyleSheet.create({
	container: {
		flex: 1,
		flexDirection: 'column-reverse',
		justifyContent: 'space-around',
		alignItems: 'center',
		justifyContent: 'center'
	},
	blueBackground: {
		backgroundColor: 'steelblue'
	},
	icon: {
		width: 200,
		height: 200
	},
	smallText: {
		fontSize: 15
	},
	redText: {
		color: 'red'
	},
	sectionHeader: {
		paddingTop: 2,
		paddingLeft: 10,
		paddingRight: 10,
		paddingBottom: 2,
		fontSize: 14,
		fontWeight: 'bold',
		backgroundColor: 'rgba(247,247,247,1.0)'
	},
	item: {
		padding: 10,
		fontSize: 18,
		height: 44
	}
});
