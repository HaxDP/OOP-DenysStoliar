using System.Text;

var builder = WebApplication.CreateBuilder(args);
var app = builder.Build();

app.MapGet("/", () =>
{
    return Results.Content(@"
        <!DOCTYPE html>
        <html>
        <head>
            <title>matrix calculator</title>
        </head>
        <body>
            <h1>matrix calculator</h1>
            <form method='post' action='/calculate'>
                <h3>matrix 1:</h3>
                <input type='number' name='m1_00' placeholder='m1[0,0]' required>
                <input type='number' name='m1_01' placeholder='m1[0,1]' required><br>
                <input type='number' name='m1_10' placeholder='m1[1,0]' required>
                <input type='number' name='m1_11' placeholder='m1[1,1]' required><br>

                <h3>matrix 2:</h3>
                <input type='number' name='m2_00' placeholder='m2[0,0]' required>
                <input type='number' name='m2_01' placeholder='m2[0,1]' required><br>
                <input type='number' name='m2_10' placeholder='m2[1,0]' required>
                <input type='number' name='m2_11' placeholder='m2[1,1]' required><br>

                <button type='submit'>calculate</button>
            </form>
        </body>
        </html>
    ", "text/html");
});

app.MapPost("/calculate", async (HttpContext context) =>
{
    var form = await context.Request.ReadFormAsync();

    Matrix m1 = new Matrix(2, 2);
    Matrix m2 = new Matrix(2, 2);

    m1[0, 0] = int.Parse(form["m1_00"]);
    m1[0, 1] = int.Parse(form["m1_01"]);
    m1[1, 0] = int.Parse(form["m1_10"]);
    m1[1, 1] = int.Parse(form["m1_11"]);

    m2[0, 0] = int.Parse(form["m2_00"]);
    m2[0, 1] = int.Parse(form["m2_01"]);
    m2[1, 0] = int.Parse(form["m2_10"]);
    m2[1, 1] = int.Parse(form["m2_11"]);

    //matrix manipulations
    Matrix m3 = m1 * m2;
    Matrix m4 = m1 + m2;

    //build result in html
    StringBuilder result = new StringBuilder();
    result.Append("<h1>results</h1>");
    result.Append("<h3>matrix m3 (m1 * m2):</h3><pre>");
    for (int i = 0; i < m3.Rows; i++)
    {
        for (int j = 0; j < m3.Columns; j++)
            result.Append(m3[i, j] + " ");
        result.Append("\n");
    }
    result.Append("</pre>");

    result.Append("<h3>matrix m4 (m1 + m2):</h3><pre>");
    for (int i = 0; i < m4.Rows; i++)
    {
        for (int j = 0; j < m4.Columns; j++)
            result.Append(m4[i, j] + " ");
        result.Append("\n");
    }
    result.Append("</pre>");

    result.Append("<a href='/'>go back</a>");

    return Results.Content(result.ToString(), "text/html");
});

app.Run();