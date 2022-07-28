// Project: IJA - Public transportation
// Authors: Michal Cibák    - xcibak00
//          Erika Fašánková - xfasan00
// Content: Class representing coordinates on a 2D map.

package transMap;

/**
* Represents position on a map using 2D coordinate system.
* The coordinates are a (x, y) pair without negative values. The map starts at (0, 0), which is its bottom left corner.
*
* @author Michal Cibák
* @author Erika Fašánková
*/
public class Coordinates
{
    private int X;
    private int Y;

    private Coordinates(int x, int y)
    {
        X = x;
        Y = y;
    }

    /**
     * Returns new coordinates if x and y aren't negative.
     * @param x - represents X coordinate
     * @param y - represents Y coordinate
     * @return new Coordinates object or NULL if it couldn't be created
     */
    public static Coordinates create(int x, int y)
    {
        if (x < 0 || y < 0)
            return null;
        else
            return new Coordinates(x, y);
    }

    @Override
    public boolean equals(Object object)
    {
        if (object instanceof Coordinates)
        {
            Coordinates object_tCoordinates = (Coordinates) object;
            if (X == object_tCoordinates.getX() && Y == object_tCoordinates.getY())
                return true;
        }

        return false;
    }

    // hashCode() should be overriden as well if something using hash code of the object is used
    /*
    @Override
    public int hashCode()
    {
        return 0;
    }
    */

    /**
     * Returns x coordinate
     * @return - x coordinate
     */
    public int getX()
    {
        return X;
    }

    /**
     * Returns y coordinate
     * @return - y coordinate
     */
    public int getY()
    {
        return Y;
    }

    /**
     * Set x coordinate according to specified value
     * @param x - specified value
     */
    public void setX(int x)
    {
        X = x >= 0 ? x : 0;
    }

    /**
     * Sets y coordinate according to specified value.
     * @param y - specified value
     */
    public void setY(int y)
    {
        Y = y >= 0 ? y : 0;
    }

    /**
     * Subtracts x coordinate of specified coordinates from x coordinate of these coordinates.
     * @param c - coordinate to be subtracted
     * @return difference of x coordinates
     */
    public int diffX(Coordinates c)
    {
        return X - c.getX();
    }

    /**
     * Subtracts y coordinate of specified coordinates from y coordinate of these coordinates.
     * @param c - coordinate to be subtracted
     * @return difference of y coordinates
     */
    public int diffY(Coordinates c)
    {
        return Y - c.getY();
    }
}
