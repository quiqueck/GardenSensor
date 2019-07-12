const   fs = require('fs')
        , path = require('path')
        , express = require("express")
        , bodyParser = require('body-parser')
        , app = express()
        , csv = require("csvtojson")
        , packageJSON = require("./package.json")
        ;

const port = 7056;
const prefix = '/api';

// parse application/x-www-form-urlencoded
app.use(bodyParser.urlencoded({ extended: false }))

// parse application/json
app.use(bodyParser.json())

app.get(prefix+'/001/time', function (req, res, next) {
    const payload = {
        ver: '001',
        date : new Date()
    }
    res.json(payload);
    next();
});

app.get(prefix+'/001/data/json', function (req, res, next) {
    const payload = {
        ver: '001',
        date : new Date(),       
    }
    csv({
        headers:['date', 'temperature', 'humidity', 'voltage', 'brightness', 'moisture', 'moisture_cached']
        , noheader:true
        , objectMode: true
        , nullObject: true
        , colParser: {
            temperature:'number'
            , humidity:'number'
            , voltage:'number'
            , brightness:'number'
            , moisture:'number'
            , moisture_cached:function(item, head, resultRow, row , colIdx){
                return (item=="true");
            }
            , date:function(item, head, resultRow, row , colIdx){
                return new Date(item);
            }}
        })
        .fromFile(path.join(__dirname, 'data.csv'))
        .then((jsonObj)=>{
            payload.data = jsonObj.map((i, ct) => {
                i["id"] = ct;
                return i;
            });
            res.json(payload);
            next();
        });
    
    
});

app.get(prefix+'/001/data', function (req, res, next) {
    const payload = {
        ver: '001',
        date : new Date(),
        data : {
            temperature: null,
            humidity: null,
            voltage: null,
            brightness: null,
            moisture: {
                value: null,
                cached: null
            }
        }
    }
    res.json(payload);
    next();
});

app.put(prefix+'/001/data', function (req, res, next) {    
    const payload = req.body;
    console.log(payload); 

    if (payload && payload.data){
        const line = `${payload.date},${payload.data.temperature},${payload.data.humidity},${payload.data.voltage},${payload.data.brightness},${payload.data.moisture.value},${payload.data.moisture.cached}\n`;
        fs.appendFile(path.join(__dirname, 'data.csv'), line, function (err) {
            if (err) {
                res.json({
                    ver: '001',
                    err: err,
                    ok : false
                });
                next(err);
            }
            console.log('Saved!');
            
            res.json({
                ver: '001',
                ok : true
            });
            next();
        });
    } else {
        res.json({
            ver: '001',
            err: "Invalid Data",
            ok : false
        });
    }
});

app.get("/js/moment.js", (request, response) => {               
    response.sendFile(path.join(__dirname, 'js/moment.js'));           
});
app.get("/js/d3.js", (request, response) => {               
    response.sendFile(path.join(__dirname, 'js/d3.v5.min.js'));           
}); 
app.get("/js/jquery.min.js", (request, response) => {               
    response.sendFile(path.join(__dirname, 'js/jquery-3.3.1.min.js'));           
}); 
app.get("/js/bootstrap.min.js", (request, response) => {               
    response.sendFile(path.join(__dirname, 'js/bootstrap.min.js'));           
});
app.get("/css/bootstrap.min.css", (request, response) => {               
    response.sendFile(path.join(__dirname, 'css/bootstrap.min.css'));           
}); 
app.get("/css/style.css", (request, response) => {               
    response.sendFile(path.join(__dirname, 'css/style.css'));           
});
app.get("/js/bootstrap.min.js.map", (request, response) => {               
    response.sendFile(path.join(__dirname, 'js/bootstrap.min.js.map'));           
});
app.get("/css/bootstrap.min.css.map", (request, response) => {               
    response.sendFile(path.join(__dirname, 'css/bootstrap.min.css.map'));           
});  

app.get("/css/bootstrap.min.css.map", (request, response) => {               
    response.sendFile(path.join(__dirname, '../css/bootstrap.min.css.map'));           
}); 
app.get("/", (request, response) => {               
    response.send(buildIndexHTML());           
}); 

app.listen(port, () => {
    console.log("Server running on port " + port);
});

function buildIndexHTML(){        
    let s = fs.readFileSync(path.join(__dirname, 'dataview.html'), { encoding: 'utf8' });

    s = s.replace('\{\{VERSION\}\}', packageJSON.version);
    return s;
}