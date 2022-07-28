// Project: IJA - Public transportation
// Authors: Michal Cibák    - xcibak00
//          Erika Fašánková - xfasan00
// Content: Class representing line of transportation.

package transMap;

import java.util.List;
import java.util.ArrayList;
import java.util.AbstractMap.SimpleImmutableEntry;

/**
* Represents one line of trasportation.
* The line has a name and list of stops and streets it covers in order of coverage.
*
* @author Michal Cibák
* @author Erika Fašánková
*/
public class Line
{
    private String Name;
    private List<Stop> StopList = new ArrayList<>();
    private List<Street> StreetList = new ArrayList<>();

    public Line(String name)
    {
        Name = name;
    }

    /**
     * Adds specified stop and its street to their respective list.
     * First stop is always added, subsequent only if the stop is on same street as previous stop or on street following street of previous stop.
     * Street is added only if it isn't last added street.
     * @param stop - specified stop
     * @return - 'true' if success, 'false' if not
     */
    public boolean addStop(Stop stop)
    {
        if (stop.getStreet() != null)
        {
            if (StopList.isEmpty())
            {
                StopList.add(stop);
                StreetList.add(stop.getStreet());
                return true;
            }
            else if (stop.getStreet().follows(StreetList.get(StreetList.size() - 1)))
            {
                StopList.add(stop);
                if (!StreetList.get(StreetList.size() - 1).equals(stop.getStreet()))
                    StreetList.add(stop.getStreet());
                return true;
            }
        }

        return false;
    }

    /**
     * Adds specified street to the list.
     * First street won't be added, line must start with a stop. Subsequent street is added if it isn't the same as last added street and it follows last added street.
     * @param street - specified street
     * @return - 'true' if success, 'false' if not
     */
    public boolean addStreet(Street street)
    {
        if (!StreetList.isEmpty() && !StreetList.get(StreetList.size() - 1).equals(street) && street.follows(StreetList.get(StreetList.size() - 1)))
        {
            StreetList.add(street);
            return true;
        }

        return false;
    }

    /**
     * Returns name of the line.
     * @return line name
     */
    public String getName()
    {
        return Name;
    }

    /**
     * Returns route of the line. Each pair consists of street and stop, if no stop should be taken on given street, null is used in place of stop.
     * @return line route
     */
    public List<SimpleImmutableEntry<Street,Stop>> getRoute()
    {
        List<SimpleImmutableEntry<Street,Stop>> route = new ArrayList<>();
        int stopindex = 0;
        for (Street street : StreetList)
        {
            if (stopindex >= StopList.size()) // no more stops
                break;
            else if (!StopList.get(stopindex).getStreet().equals(street)) // no stops on given street
                route.add(new SimpleImmutableEntry<>(street, null));
            else // there is at least one stop on given street
                do
                    route.add(new SimpleImmutableEntry<>(street, StopList.get(stopindex++))); // add "street, stop" pair to route
                while (StopList.size() > stopindex && StopList.get(stopindex).getStreet().equals(street)); // there are more stops on given street
        }
        return route;
    }
}
