import * as express from 'express';
import routes from './routes/index';
import gpt from './routes/gpt';

const app = express();

app.use(express.json({limit: "10mb"}));
app.use((req, res, next) => { console.log(req.method, req.url); next(); });
app.use('/', routes);
app.use('/gpt', gpt);
app.use('/health', (req, res) => res.send("still alive..."));

app.use((req, res, next) => {
    const err = new Error('Not Found');
    err[ 'status' ] = 404;
    next(err);
});

// development error handler
// will print stacktrace
if (app.get('env') === 'development') {
    app.use((err, req, res, next) => { // eslint-disable-line @typescript-eslint/no-unused-vars
        res.status(err[ 'status' ] || 500);
    });
}

// production error handler
// no stacktraces leaked to user
app.use((err, req, res, next) => { // eslint-disable-line @typescript-eslint/no-unused-vars
    console.log("error!!");
    res.status(err.status || 500);
});

app.set('port', process.env.PORT || 3010);

const server = app.listen(app.get('port'), function () {
    console.log("Server is listening on", app.get('port'), "port");
});