public class Matrix
{
    private double[,] data;

    public int Rows { get; }
    public int Columns { get; }

    public Matrix(int rows, int columns)
    {
        Rows = rows;
        Columns = columns;
        data = new double[rows, columns];
    }

    public double this[int row, int col]
    {
        get => data[row, col];
        set => data[row, col] = value;
    }

    //operator
    public static Matrix operator *(Matrix a, Matrix b)
    {
        if (a.Columns != b.Rows)
            throw new InvalidOperationException("matrix dimensions do not match for multiplication(");

        Matrix result = new Matrix(a.Rows, b.Columns);
        for (int i = 0; i < a.Rows; i++)
            for (int j = 0; j < b.Columns; j++)
                for (int k = 0; k < a.Columns; k++)
                    result[i, j] += a[i, k] * b[k, j];
        return result;
    }

    //operator addition
    public static Matrix operator +(Matrix a, Matrix b)
    {
        if (a.Rows != b.Rows || a.Columns != b.Columns)
            throw new InvalidOperationException("matrix dimensions must match for addition(");

        Matrix result = new Matrix(a.Rows, a.Columns);
        for (int i = 0; i < a.Rows; i++)
            for (int j = 0; j < a.Columns; j++)
                result[i, j] = a[i, j] + b[i, j];
        return result;
    }
}