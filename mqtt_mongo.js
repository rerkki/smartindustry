
//MQTT Brpker definitions
const mqtt    = require('mqtt');
const broker = 'mqtt://test.mosquitto.org';
const user = '';
const pw = ''; 

// Give unique names for database and collection (MongoDB Atlas)
const dbname = "sensordata";
const collection = "sensordata";

//Connect to broker
mq = mqtt.connect(broker, {
  'username': user,
  'password': pw
});

//subscribe the topic
mq.subscribe('automaatio1/#');

//dotify about successful connection
mq.on('connect', function(){
    console.log('Connected.....');
});

//API for MongoDB Atlas
const { MongoClient, ServerApiVersion } = require('mongodb');

//korvaa alla oleva URI-string omalla URI:lla (hae se Mongo Atlaksen Connect-kohdasta, lisää myös oma käyttäjätunnus ja salasana)
const uri = "mongodb+srv://<username>:<password>@clusterxxxxx.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0"; 

const client = new MongoClient(uri, {
  serverApi: {
    version: ServerApiVersion.v1,
    strict: true,
    deprecationErrors: true,
  }
});

//Definition of database & collection and and object for data retrieval/storage
const myDB = client.db(dbname);
const myColl = myDB.collection(collection);
var obj;

//wait for data from MQTT broker and insert it to MongoDB
mq.on('message', function(topic, message) {
  console.log(message.toString('utf8'));
  obj = JSON.parse(message);
  obj.DateTime = timeConverter(Date.now());
	myColl.insertOne(obj);
	console.log(
	`An entry was inserted successfully`,
	);
});

if (!Date.now) {
    Date.now = function() { return new Date().getTime(); }
}

function timeConverter(UNIX_timestamp){
  var a = new Date(UNIX_timestamp);
  var months = ['Jan','Feb','Mar','Apr','May','Jun','Jul','Aug','Sep','Oct','Nov','Dec'];
  var year = a.getFullYear();
  var month = months[a.getMonth()];
  var date = a.getDate();
  var hour = a.getHours();
  var min = a.getMinutes();
  var sec = a.getSeconds();
  var time = date + ' ' + month + ' ' + year + ' ' + hour + ':' + min + ':' + sec ;
  return time;
}

