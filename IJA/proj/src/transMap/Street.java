// Project: IJA - Public transportation
// Authors: Michal Cibák    - xcibak00
//          Erika Fašánková - xfasan00
// Content: Class representing street on a map.

package transMap;

import java.util.List;
import java.util.ArrayList;

/**
* Represents one street on a map.
* The street has a name and is defined by at least 2 coordinates. All parts of the street must be either horizontal or vertical.
* It can contain stops.
*
* @author Michal Cibák
* @author Erika Fašánková
*/
public class Street
{
    private String Name;
    private List<Coordinates> CoordinatesList = new ArrayList<>();
    private List<Stop> StopList = new ArrayList<>();
    private int Speed = 3;

    private Street(String name, Coordinates... coordinates_list)
    {
        Name = name;
        for (Coordinates coordinates : coordinates_list)
            CoordinatesList.add(coordinates);
    }

    /**
     * Creates a new street with specified name and coordinates, with default speed and no stops.
     * Parts of the street must be straight or rightangled.
     * @param name - specified street name
     * @param coordinates_list - list of coordinates of the street
     * @return - new street or NULL when the street can't be created
     */
    static public Street create(String name, Coordinates... coordinates_list)
    {
        if (coordinates_list.length >= 2)
        {
            boolean rightangled = true;
            Coordinates coordPrevious = coordinates_list[0];
            for (Coordinates coordActual : coordinates_list)
            {
                if (coordActual.getX() != coordPrevious.getX() && coordActual.getY() != coordPrevious.getY())
                {
                    rightangled = false;
                    break;
                }
                coordPrevious = coordActual;
            }

            if (rightangled)
                return new Street(name, coordinates_list);
        }

        return null;
    }

    @Override
    public boolean equals(Object object)
    {
        if (object instanceof Street)
        {
            Street object_Street = (Street) object;
            if (Name.equals(object_Street.Name))
                return true;
        }

        return false;
    }

    /**
     * Returns name of the street.
     * @return - street name
     */
    public String getName()
    {
        return Name;
    }

    /**
     * Returns a list of coordinates that specify the street.
     * @return - coordinates list
     */
    public List<Coordinates> getCoordinatesList()
    {
        return CoordinatesList;
    }

    /**
     * Returns a list of stops that lay on the street.
     * @return - stop list
     */
    public List<Stop> getStops()
    {
        return StopList;
    }

    /**
     * Returns speed on the street.
     * @return - street speed
     */
    public int getSpeed()
    {
        return Speed;
    }

    /**
     * Sets speed on the street.
     * @param speed - street speed
     */
    public void setSpeed(int speed)
    {
        Speed = speed;
    }

    /**
     * Returns stop with specified name.
     * @param name - specified name
     * @return - corresponding stop or NULL if stop doesn't exist
     */
    public Stop getStopByName(String name)
    {
        Stop namedStop = null;
        for (Stop stop : StopList)
        {
            if (stop.getName().equals(name))
            {
                namedStop = stop;
                break;
            }
        }
        return namedStop;
    }

    /**
     * Adds specified stop to the street if it lays on it.
     * @param stop - stop which should be added
     * @return - 'true' on success, 'false' if not success
     */
    public boolean addStop(Stop stop)
    {
        if (stop.getCoordinates() != null)
        {
            Coordinates coordPrevious;
            Coordinates coordActual = this.getBeginning();
            for (int i = 1; i < CoordinatesList.size(); ++i)
            {
                coordPrevious = coordActual;
                coordActual = CoordinatesList.get(i);
                if (coordActual.diffX(coordPrevious) == 0 && stop.getCoordinates().diffX(coordActual) == 0 &&
                        !(stop.getCoordinates().getY() > coordActual.getY() && stop.getCoordinates().getY() > coordPrevious.getY()) &&
                        !(stop.getCoordinates().getY() < coordActual.getY() && stop.getCoordinates().getY() < coordPrevious.getY()) ||
                    coordActual.diffY(coordPrevious) == 0 && stop.getCoordinates().diffY(coordActual) == 0 &&
                        !(stop.getCoordinates().getX() > coordActual.getX() && stop.getCoordinates().getX() > coordPrevious.getX()) &&
                        !(stop.getCoordinates().getX() < coordActual.getX() && stop.getCoordinates().getX() < coordPrevious.getX())
                )
                {
                    StopList.add(stop);
                    stop.setStreet(this);
                    return true;
                }
            }
        }

        return false;
    }

    /**
     * Returns coordinates of the beginning of the street.
     * @return - beginning street coordinates
     */
    public Coordinates getBeginning()
    {
        return CoordinatesList.get(0);
    }

    /**
     * Returns coordinates of the end of the street.
     * @return - end street coordinates
     */
    public Coordinates getEnd()
    {
        return CoordinatesList.get(CoordinatesList.size() - 1);
    }

    /**
     * Checks if coordinates of beginning or end of this street correspond to coordinates of beginning or end of specified street.
     * If it does, the streets either follow each other or are the same street.
     * @param street - specified street
     * @return - 'true' if follows, 'false' if not
     */
    public boolean follows(Street street)
    {
        if (street != null)
            if (this.getBeginning().equals(street.getBeginning()) ||
                this.getBeginning().equals(street.getEnd()) ||
                this.getEnd().equals(street.getBeginning()) ||
                this.getEnd().equals(street.getEnd())
            )
                return true;

        return false;
    }
}
