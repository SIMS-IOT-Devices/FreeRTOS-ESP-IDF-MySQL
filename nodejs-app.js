const app = require("express")();
const mysql = require('mysql');

// In order to read and parse the JSON POST request
var bodyParser = require('body-parser');
app.use(bodyParser.json());

// MySQL database connection
var db = mysql.createConnection({
  host: 'localhost',
  user: 'root',
  password: '123456',
  database: 'db_v1'
  // table: 'table_v1'
})
db.connect(function(err) {
  if (err) throw err;
  console.log("Connected!");
  let millis = Date.now();
  console.log("Current time in millis: ", millis);
});

// Start listening to specific port
app.listen(8085, () => console.log("Listening to port 8085."));

// REST GET request ------------------------------------------------
app.get('/get', (req, res) => { 
  res.send("Get function was activated ...");

  // Read the data from MySQL table
  console.log("MySQL table data:");
  let sql_get = 'SELECT * FROM table_v1';
  db.query(sql_get,(err, result) => {
    if (err) throw err;
    console.log(result);
  });
});

// REST POST request -----------------------------------------------
app.post('/post', (req, res) => { 
  res.send("Post function was activated ...");

  // Build the data block
  let millis = Date.now();
  console.log("Postman massage body: ", req.body);
  console.log("Postman massage body time: ", millis);
  console.log("Postman massage body data: ", req.body.data);
  let post_data = {time: millis, data: req.body.data};
 
  // Send the data block to MySQL table
  let sql_post = 'INSERT INTO table_v1 SET ?';
  db.query(sql_post, post_data, (err, result) => {
    if (err) throw err;
  });
});
