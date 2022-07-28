// Project: IJA - Public transportation
// Authors: Michal Cibák    - xcibak00
//          Erika Fašánková - xfasan00
// Content: Class representing stop on a street.

package transMap;

/**
* Represents one stop on a street.
* The stop has a name and coordinates, it can know the street it is on.
*
* @author Michal Cibák
* @author Erika Fašánková
*/
public class Stop
{
    private String Name;
    private Coordinates Coordinates;
    private Street Street;

    public Stop(String name, Coordinates coords)
    {
        Name = name;
        Coordinates = coords;
    }

    @Override
    public boolean equals(Object object)
    {
        if (object instanceof Stop)
        {
            Stop object_Stop = (Stop) object;
            if (Name.equals(object_Stop.Name))
                return true;
        }

        return false;
    }

/*
    @Override
    public String toString()
    {
        return "stop(" + Name + ")";
    }
*/

    /**
     * Returns name of the stop.
     * @return - stop name
     */
    public String getName()
    {
        return Name;
    }

    /**
     * Returns coordinates of the stop.
     * @return - stop coordinates
     */
    public Coordinates getCoordinates()
    {
        return Coordinates;
    }

    /**
     * Set the street on which the stop is located.
     * @param s - street
     */
    public void setStreet(Street s)
    {
        Street = s;
    }

    /**
     * Returns the street where the stop is located.
     * If the stop has no coordinates, NULL is returned.
     * @return - corresponding street or NULL
     */
    public Street getStreet()
    {
        if (Coordinates == null)
            return null;
        else
            return Street;
    }
}
