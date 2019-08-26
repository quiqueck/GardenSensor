const fs = require("fs"),
  path = require("path"),
  express = require("express"),
  bodyParser = require("body-parser"),
  app = express(),
  csv = require("csvtojson"),
  packageJSON = require("./package.json"),
  cors = require('cors');
const port = 7056;
const prefix = "/api";

// parse application/x-www-form-urlencoded
app.use(bodyParser.urlencoded({ extended: false }));

// parse application/json
app.use(bodyParser.json());

var allowedOrigins = ['http://localhost:'+port,
                      'http://127.0.0.1:'+port,
                      'http://192.168.55.21:'+port];
app.use(cors({
  origin: function(origin, callback){
    // allow requests with no origin 
    // (like mobile apps or curl requests)
    if(!origin) return callback(null, true);
    if(allowedOrigins.indexOf(origin) === -1){
      var msg = 'The CORS policy for this site does not ' +
                'allow access from the specified Origin.';
      return callback(new Error(msg), false);
    }
    return callback(null, true);
  }
}));

// app.use(function (req, res, next) {
//     const ip = req.headers['x-forwarded-for'] || req.connection.remoteAddress;
//     const host = req.headers.host;
//     console.log(ip, host, 'http://' + host);
//     // Website you wish to allow to connect
//     res.setHeader('Access-Control-Allow-Origin', 'http://' + host);

//     // Request methods you wish to allow
//     res.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS, PUT, PATCH, DELETE');

//     // Request headers you wish to allow
//     res.setHeader('Access-Control-Allow-Headers', 'X-Requested-With,content-type');

//     // Set to true if you need the website to include cookies in the requests sent
//     // to the API (e.g. in case you use sessions)
//     res.setHeader('Access-Control-Allow-Credentials', true);

//     // Pass to next layer of middleware
//     next();
// });

app.use('/', express.static(path.join(__dirname, "dist")));

app.get(prefix + "/001/time", function(req, res, next) {
  const payload = {
    ver: "001",
    date: new Date()
  };
  res.json(payload);
  next();
});

function loadData(fkt) {
  csv({
    headers: [
      "date",
      "temperature",
      "humidity",
      "voltage",
      "brightness",
      "moisture",
      "moisture_cached",
      "brightness_mt"
    ],
    noheader: true,
    objectMode: true,
    nullObject: true,
    colParser: {
      temperature: "number",
      humidity: "number",
      voltage: "number",
      brightness: "number",
      brightness_mt: "number",
      moisture: "number",
      moisture_cached: function(item, head, resultRow, row, colIdx) {
        return item == "true";
      },
      date: function(item, head, resultRow, row, colIdx) {
        return new Date(item);
      }
    }
  })
    .fromFile(path.join(__dirname, "data.csv"))
    .then(fkt);
}

app.get(prefix + "/001/data/json", function(req, res, next) {
  const payload = {
    ver: "001",
    date: new Date()
  };
  loadData(jsonObj => {
    payload.data = jsonObj.map((i, ct) => {
      i["id"] = ct;
      return i;
    });
    res.json(payload);
    next();
  });
});

app.get(prefix + "/001/data/json/latest/:itemCount", function(req, res, next) {
  const payload = {
    ver: "001",
    date: new Date(),
    latest: req.params.itemCount
  };
  loadData(jsonObj => {
    payload.data = jsonObj.map((i, ct) => {
      i["id"] = ct;
      return i;
    });
    payload.data = payload.data.slice(
      Math.max(0, payload.data.length - req.params.itemCount)
    );
    res.json(payload);
    next();
  });
});

app.get(prefix + "/001/data/json/afterID/:itemID", function(req, res, next) {
  const payload = {
    ver: "001",
    date: new Date(),
    afterID: req.params.itemID
  };
  loadData(jsonObj => {
    payload.data = jsonObj.map((i, ct) => {
      i["id"] = ct;
      return i;
    });
    payload.data = payload.data.filter(i => {
      return i["id"] > req.params.itemID;
    });
    res.json(payload);
    next();
  });
});

app.get(prefix + "/001/data/json/after/:aTime", function(req, res, next) {
  const payload = {
    ver: "001",
    date: new Date(),
    after: new Date(req.params.aTime)
  };
  loadData(jsonObj => {
    payload.data = jsonObj.map((i, ct) => {
      i["id"] = ct;
      return i;
    });
    payload.data = payload.data.filter(i => {
      return i.date > payload.after;
    });
    res.json(payload);
    next();
  });
});

app.get(prefix + "/001/data/json/before/:bTime", function(req, res, next) {
  const payload = {
    ver: "001",
    date: new Date(),
    before: new Date(req.params.bTime)
  };
  loadData(jsonObj => {
    payload.data = jsonObj.map((i, ct) => {
      i["id"] = ct;
      return i;
    });
    payload.data = payload.data.filter(i => {
      return i.date < payload.before;
    });
    res.json(payload);
    next();
  });
});

app.get(prefix + "/001/data/json/after/:aTime/before/:bTime", function(
  req,
  res,
  next
) {
  const payload = {
    ver: "001",
    date: new Date(),
    before: new Date(req.params.bTime),
    after: new Date(req.params.aTime)
  };
  loadData(jsonObj => {
    payload.data = jsonObj.map((i, ct) => {
      i["id"] = ct;
      return i;
    });
    payload.data = payload.data.filter(i => {
      return i.date < payload.before && i.date > payload.after;
    });
    res.json(payload);
    next();
  });
});

app.get(prefix + "/001/data", function(req, res, next) {
  const payload = {
    ver: "001",
    date: new Date(),
    data: {
      temperature: null,
      humidity: null,
      voltage: null,
      brightness: {
        value: null,
        mt: null
      },
      moisture: {
        value: null,
        cached: null
      }
    }
  };
  res.json(payload);
  next();
});

app.put(prefix + "/001/data", function(req, res, next) {
  const payload = req.body;
  console.log(payload);

  if (payload && payload.data) {
    const line = `${payload.date},${payload.data.temperature},${payload.data.humidity},${payload.data.voltage},${payload.data.brightness.value},${payload.data.moisture.value},${payload.data.moisture.cached},${payload.data.brightness.mt}\n`;
    fs.appendFile(path.join(__dirname, "data.csv"), line, function(err) {
      if (err) {
        res.json({
          ver: "001",
          err: err,
          ok: false
        });
        next(err);
      }
      console.log("Saved!");

      res.json({
        ver: "001",
        ok: true
      });
      next();
    });
  } else {
    res.json({
      ver: "001",
      err: "Invalid Data",
      ok: false
    });
  }
});

app.get("/js/moment.js", (request, response) => {
  response.sendFile(path.join(__dirname, "js/moment.js"));
});
app.get("/js/d3.js", (request, response) => {
  response.sendFile(path.join(__dirname, "js/d3.v5.min.js"));
});
app.get("/js/jquery.min.js", (request, response) => {
  response.sendFile(path.join(__dirname, "js/jquery-3.3.1.min.js"));
});
app.get("/js/bootstrap.min.js", (request, response) => {
  response.sendFile(path.join(__dirname, "js/bootstrap.min.js"));
});
app.get("/css/bootstrap.min.css", (request, response) => {
  response.sendFile(path.join(__dirname, "css/bootstrap.min.css"));
});
app.get("/css/style.css", (request, response) => {
  response.sendFile(path.join(__dirname, "css/style.css"));
});
app.get("/js/bootstrap.min.js.map", (request, response) => {
  response.sendFile(path.join(__dirname, "js/bootstrap.min.js.map"));
});
app.get("/css/bootstrap.min.css.map", (request, response) => {
  response.sendFile(path.join(__dirname, "css/bootstrap.min.css.map"));
});

app.get("/css/bootstrap.min.css.map", (request, response) => {
  response.sendFile(path.join(__dirname, "../css/bootstrap.min.css.map"));
});
app.get("/old", (request, response) => {
  response.send(buildIndexHTML());
});

app.listen(port, () => {
  console.log("Server running on port " + port);
});

function buildIndexHTML() {
  let s = fs.readFileSync(path.join(__dirname, "dataview.html"), {
    encoding: "utf8"
  });

  s = s.replace("{{VERSION}}", packageJSON.version);
  return s;
}
