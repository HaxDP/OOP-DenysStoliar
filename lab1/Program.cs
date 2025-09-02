using Lab1;

var builder = WebApplication.CreateBuilder(args);
var app = builder.Build();

app.MapGet("/", (HttpContext context) =>
{
    context.Response.ContentType = "text/html";
    return @"
    <html>
      <body>
        <h1>hehe.sql</h1>
        <button onclick=""location.href='/figure'"">Go to Figure</button>
      </body>
    </html>
    ";
});

app.MapGet("/figure", (HttpContext context) =>
{
    context.Response.ContentType = "text/html";
    var figure = new Figure(228.0);
    return $@"
    <html>
      <body>
        <h1>{figure.GetFigure()}</h1>
        <button onclick=""location.href='/'"">Back</button>
      </body>
    </html>
    ";
});

app.Run();