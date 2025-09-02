namespace Lab1
{
    public class Figure
    {
        //private
        private double _area;

        //public
        public double Area
        {
            get { return _area; }
            set { _area = value; }
        }

        //constructor
        public Figure(double area)
        {
            _area = area;
        }

        //cleaner
        ~Figure()
        { }

        //method
        public string GetFigure()
        {
            return $"figure with area: {Area}";
        }
    }
}