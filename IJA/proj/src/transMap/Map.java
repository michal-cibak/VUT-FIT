// Project: IJA - Public transportation
// Authors: Michal Cibák    - xcibak00
//          Erika Fašánková - xfasan00
// Content: Class representing a controller of the program.

package transMap;

import java.util.ArrayList;
import java.util.List;

/**
* Represents a map.
* The map has a name.
* It can contain streets, lines and vehicles of public transportation.
*
* @author Michal Cibák
* @author Erika Fašánková
*/
public class Map
{
    private String Name;
    private List<Street> StreetList = new ArrayList<>();
    private List<Line> LineList = new ArrayList<>();
    private List<Tram> TramList = new ArrayList<>();

    public Map()
    {
        Name = "Unnamed map";
    }

    public Map(String name)
    {
        Name = name;
    }

    /**
     * Returns name of the map.
     * @return map name
     */
    public String getName()
    {
        return Name;
    }

    /**
     * Adds specified street to the street list.
     * @param street - specified street
     */
    public void addStreet(Street street) // TODO: could return boolean - it would fail if street with such name already exists
    {
        StreetList.add(street);
    }

    /**
     * Returns list of the streets on the map.
     * @return - street list
     */
    public List<Street> getStreets()
    {
        return StreetList;
    }

    /**
     * Finds stop with a specified name on the streets of the map.
     * @param stopName - specified stop name
     * @return - stop or NULL if stop doesn't exist
     */
    public Stop getStopByName(String stopName)
    {
        Stop namedStop = null;
        for (Street street : StreetList)
        {
            namedStop = street.getStopByName(stopName);
            if (namedStop != null)
                break;
        }
        return namedStop;
    }

    /**
     * Finds street with a specified name on the streets of the map.
     * @param streetName - specified street name
     * @return - street or NULL if street doesn't exist
     */
    public Street getStreetByName(String streetName)
    {
        Street namedStreet = null;
        for (Street street : StreetList)
        {
            if (street.getName().equals(streetName))
            {
                namedStreet = street;
                break;
            }
        }
        return namedStreet;
    }

    /**
     * Adds specified line to the list of lines.
     * @param line - specified line
     */
    public void addLine(Line line) // TODO: could return boolean - it would fail if line with such name already exists
    {
        LineList.add(line);
    }

    /**
     * Returns list of the lines on the map.
     * @return - street list
     */
    public List<Line> getLines()
    {
        return LineList;
    }

    /**
     * Finds line with a specified name on the map.
     * @param lineName - specified name
     * @return - line or NULL if line doesn't exits
     */
    public Line getLineByName(String lineName)
    {
        Line namedLine = null;
        for (Line line : LineList)
        {
            if (line.getName().equals(lineName))
            {
                namedLine = line;
                break;
            }
        }
        return namedLine;
    }

    /**
     * Adds specified tram to the list of trams.
     * @param tram - specified tram
     */
    public void addTram(Tram tram)
    {
        TramList.add(tram);
    }

    /**
     * Returns list of the trams on the map.
     * @return - tram list
     */
    public List<Tram> getTrams()
    {
        return TramList;
    }
}
