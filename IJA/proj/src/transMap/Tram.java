// Project: IJA - Public transportation
// Authors: Michal Cibák    - xcibak00
//          Erika Fašánková - xfasan00
// Content: Class representing vehicle corresponding to a specific line.

package transMap;

import java.time.LocalTime;
import java.util.AbstractMap.SimpleImmutableEntry;
import java.util.List;
import java.util.ArrayList;
import java.util.AbstractMap.SimpleEntry;

/**
* Represents one vehicle of public transportation.
* The tram knows its route, current position, ...
*
* @author Michal Cibák
* @author Erika Fašánková
*/
public class Tram
{
    private String Name;
    private List<SimpleImmutableEntry<Street,Stop>> Route;
    private boolean RouteStartToEnd;
    private List<Coordinates> Path = new ArrayList<>();
    private int Delay = 0; // should be calculated after arriving to a stop as real delay or after entering a detour as current delay plus delay of the detour
    private int WaitingTime = 20;
    private int Waiting = WaitingTime;
    private Coordinates Position;
    private Street CurrentStreet;
    // private int CurrentStreetIndex;
    private int NextPositionIndex;
    private int NextStopIndex;
    private Stop LastStop;
    private boolean Traveling = true;
    private List<SimpleEntry<List<Coordinates>,Integer>> Detours = new ArrayList<>();
    private ArrayList<String> Timetable = new ArrayList<String>();
    private List<Integer> DepartureTimes = new ArrayList<>();
    private int NextDepartureIndex = 0;
    private int TravelTime = 0;

    public Tram(String tramName, List<SimpleImmutableEntry<Street,Stop>> route, boolean direction, ArrayList<String> timetable) // route with at least 2 stops is assumed
    {
        Name = tramName;
        Route = route;
        RouteStartToEnd = direction;
        Timetable = timetable;

        // make list of departure times
        String[] splitFirstTime = timetable.get(0).split("\\.");
        int firstTime = LocalTime.of(Integer.parseInt(splitFirstTime[0]), Integer.parseInt(splitFirstTime[1])).toSecondOfDay();
        DepartureTimes.add(0);
        for (int i = 1; i < timetable.size(); ++i)
        {
            String[] splitTime = timetable.get(i).split("\\.");
            int currentTime = LocalTime.of(Integer.parseInt(splitTime[0]), Integer.parseInt(splitTime[1])).toSecondOfDay();
            DepartureTimes.add(currentTime - firstTime);
        }
        DepartureTimes.add(0);

        // make path - list of coordinates at which point of interest (stop, crossroad) is located // TODO - only works with straight streets and doesn't expect stop on crossroad
        Street lastStreet = Route.get(0).getKey();
        Street currentStreet;
        Stop lastStop = Route.get(0).getValue();
        Stop currentStop;
        Path.add(lastStop.getCoordinates());
        for (int routeIndex = 1; routeIndex < Route.size(); ++routeIndex)
        {
            currentStreet = Route.get(routeIndex).getKey();
            currentStop = Route.get(routeIndex).getValue();
            if (currentStreet == lastStreet) // this street is the same as the last street
            {
                if (lastStop != null) // there was a stop on this street before
                {
                    if (currentStop != null) // there is a stop now
                    // add coordinates of crossroads between them and coordinates of the stop to the path
                    {
                        boolean currAfterPrev;
                        boolean ascending;
                        if (currentStreet.getBeginning().diffX(currentStreet.getEnd()) != 0) // horizontal street
                        {
                            currAfterPrev = currentStop.getCoordinates().diffX(lastStop.getCoordinates()) > 0 ? true : false;
                            ascending = currentStreet.getEnd().diffX(currentStreet.getBeginning()) > 0 ? true : false;
                            // TODO - bodies of both fors are the same, 1 while could be used instead
                            if (currAfterPrev == ascending)
                            {
                                for (int i = 0; i < currentStreet.getCoordinatesList().size(); ++i)
                                {
                                    Coordinates coords = currentStreet.getCoordinatesList().get(i);
                                    if (!(coords.getX() > lastStop.getCoordinates().getX() && coords.getX() > currentStop.getCoordinates().getX()) &&
                                        !(coords.getX() < lastStop.getCoordinates().getX() && coords.getX() < currentStop.getCoordinates().getX())
                                    ) // the point on street is between the stops
                                        Path.add(coords);
                                }
                            }
                            else
                            {
                                for (int i = currentStreet.getCoordinatesList().size() - 1; i >= 0; --i)
                                {
                                    Coordinates coords = currentStreet.getCoordinatesList().get(i);
                                    if (!(coords.getX() > lastStop.getCoordinates().getX() && coords.getX() > currentStop.getCoordinates().getX()) &&
                                        !(coords.getX() < lastStop.getCoordinates().getX() && coords.getX() < currentStop.getCoordinates().getX())
                                    ) // the point on street is between the stops
                                        Path.add(coords);
                                }
                            }
                        }
                        else // vertical street
                        {
                            currAfterPrev = currentStop.getCoordinates().diffY(lastStop.getCoordinates()) > 0 ? true : false;
                            ascending = currentStreet.getEnd().diffY(currentStreet.getBeginning()) > 0 ? true : false;
                            // TODO - bodies of both fors are the same, 1 while could be used instead
                            if (currAfterPrev == ascending)
                            {
                                for (int i = 0; i < currentStreet.getCoordinatesList().size(); ++i)
                                {
                                    Coordinates coords = currentStreet.getCoordinatesList().get(i);
                                    if (!(coords.getY() > lastStop.getCoordinates().getY() && coords.getY() > currentStop.getCoordinates().getY()) &&
                                        !(coords.getY() < lastStop.getCoordinates().getY() && coords.getY() < currentStop.getCoordinates().getY())
                                    ) // the point on street is between the stops
                                        Path.add(coords);
                                }
                            }
                            else
                            {
                                for (int i = currentStreet.getCoordinatesList().size() - 1; i >= 0; --i)
                                {
                                    Coordinates coords = currentStreet.getCoordinatesList().get(i);
                                    if (!(coords.getY() > lastStop.getCoordinates().getY() && coords.getY() > currentStop.getCoordinates().getY()) &&
                                        !(coords.getY() < lastStop.getCoordinates().getY() && coords.getY() < currentStop.getCoordinates().getY())
                                    ) // the point on street is between the stops
                                        Path.add(coords);
                                }
                            }
                        }
                        Path.add(currentStop.getCoordinates());
                    }
                    else // there is no more stops on this street - error (it shouldn't happen)
                    {
                        System.out.println("Error in path making - stop before, but no more on the same street.");
                    }
                }
                else // there was no stop on this street before - error (it shouldn't happen)
                {
                    System.out.println("Error in path making - no stop before on the same street.");
                }
            }
            else // this street isn't the same as the last street
            {
                // add remaining coordinates from previous street to the path if needed
                if (lastStop != null) // there was a stop on the last street
                // coordinates of crossroads on last street after the last stop and end of the street aren't in the path, add them
                {
                    boolean lastEndToCurrent = lastStreet.getEnd().equals(currentStreet.getBeginning()) || lastStreet.getEnd().equals(currentStreet.getEnd()) ? true : false;
                    boolean ascending;
                    if (lastStreet.getBeginning().diffX(lastStreet.getEnd()) != 0) // horizontal street
                    {
                        ascending = lastStreet.getEnd().diffX(lastStreet.getBeginning()) > 0 ? true : false;
                        // TODO - the fors are similar, maybe it could be done using 1 while
                        if (lastEndToCurrent) // last streets end is connected to current street
                        {
                            for (int i = 0; i < lastStreet.getCoordinatesList().size(); ++i)
                            {
                                Coordinates coords = lastStreet.getCoordinatesList().get(i);
                                if (ascending)
                                {
                                    if (coords.getX() > lastStop.getCoordinates().getX()) // the point on the street is after the stop
                                        Path.add(coords);
                                }
                                else
                                {
                                    if (coords.getX() < lastStop.getCoordinates().getX()) // the point on the street is after the stop
                                        Path.add(coords);
                                }
                            }
                        }
                        else // last streets beginning is connected to current street
                        {
                            for (int i = lastStreet.getCoordinatesList().size() - 1; i >= 0; --i)
                            {
                                Coordinates coords = lastStreet.getCoordinatesList().get(i);
                                if (ascending)
                                {
                                    if (coords.getX() < lastStop.getCoordinates().getX()) // the point on the street is after the stop
                                        Path.add(coords);
                                }
                                else
                                {
                                    if (coords.getX() > lastStop.getCoordinates().getX()) // the point on the street is after the stop
                                        Path.add(coords);
                                }
                            }
                        }
                    }
                    else // vertical street
                    {
                        ascending = lastStreet.getEnd().diffY(lastStreet.getBeginning()) > 0 ? true : false;
                        // TODO - the fors are similar, maybe it could be done using 1 while
                        if (lastEndToCurrent) // last streets end is connected to current street
                        {
                            for (int i = 0; i < lastStreet.getCoordinatesList().size(); ++i)
                            {
                                Coordinates coords = lastStreet.getCoordinatesList().get(i);
                                if (ascending)
                                {
                                    if (coords.getY() > lastStop.getCoordinates().getY()) // the point on the street is after the stop
                                        Path.add(coords);
                                }
                                else
                                {
                                    if (coords.getY() < lastStop.getCoordinates().getY()) // the point on the street is after the stop
                                        Path.add(coords);
                                }
                            }
                        }
                        else // last streets beginning is connected to current street
                        {
                            for (int i = lastStreet.getCoordinatesList().size() - 1; i >= 0; --i)
                            {
                                Coordinates coords = lastStreet.getCoordinatesList().get(i);
                                if (ascending)
                                {
                                    if (coords.getY() < lastStop.getCoordinates().getY()) // the point on the street is after the stop
                                        Path.add(coords);
                                }
                                else
                                {
                                    if (coords.getY() > lastStop.getCoordinates().getY()) // the point on the street is after the stop
                                        Path.add(coords);
                                }
                            }
                        }
                    }
                }
                // else {;} // there was no stop on the last street - all coordinates from last street are in the path, no need to add them

                // add coordinates from this street to the path
                if (currentStreet.getBeginning().equals(Path.get(Path.size() - 1))) // beginning of this street is the last added position
                {
                    if (currentStop != null) // this street has at least one stop
                    // add coordinates of crossroads before the first stop and coordinates of the stop to the path
                    {
                        boolean ascending;
                        if (currentStreet.getBeginning().diffX(currentStreet.getEnd()) != 0) // horizontal street
                        {
                            ascending = currentStreet.getEnd().diffX(currentStreet.getBeginning()) > 0 ? true : false;
                            // TODO - the fors are similar, maybe it could be done using 1 while
                            for (int i = 1; i < currentStreet.getCoordinatesList().size(); ++i)
                            {
                                Coordinates coords = currentStreet.getCoordinatesList().get(i);
                                if (ascending)
                                {
                                    if (coords.getX() < currentStop.getCoordinates().getX()) // the point on the street is after the stop
                                        Path.add(coords);
                                }
                                else
                                {
                                    if (coords.getX() > currentStop.getCoordinates().getX()) // the point on the street is after the stop
                                        Path.add(coords);
                                }
                            }
                        }
                        else // vertical street
                        {
                            ascending = currentStreet.getEnd().diffY(currentStreet.getBeginning()) > 0 ? true : false;
                            // TODO - the fors are similar, maybe it could be done using 1 while
                            for (int i = 1; i < currentStreet.getCoordinatesList().size(); ++i)
                            {
                                Coordinates coords = currentStreet.getCoordinatesList().get(i);
                                if (ascending)
                                {
                                    if (coords.getY() < currentStop.getCoordinates().getY()) // the point on the street is after the stop
                                        Path.add(coords);
                                }
                                else
                                {
                                    if (coords.getY() > currentStop.getCoordinates().getY()) // the point on the street is after the stop
                                        Path.add(coords);
                                }
                            }
                        }
                        Path.add(currentStop.getCoordinates());
                    }
                    else // this street has no stops
                    // add the coordinates of the whole street (except the meeting point with last street) to the path
                    {
                        for (int i = 1; i < currentStreet.getCoordinatesList().size(); ++i)
                        {
                            Path.add(currentStreet.getCoordinatesList().get(i));
                        }
                    }
                }
                else // end of this street is the last added position
                {
                    if (currentStop != null) // this street has at least one stop
                    // add coordinates of crossroads before the first stop and coordinates of the stop to the path
                    {
                        boolean ascending;
                        if (currentStreet.getBeginning().diffX(currentStreet.getEnd()) != 0) // horizontal street
                        {
                            ascending = currentStreet.getEnd().diffX(currentStreet.getBeginning()) > 0 ? true : false;
                            // TODO - the fors are similar, maybe it could be done using 1 while
                            for (int i = currentStreet.getCoordinatesList().size() - 2; i >= 0; --i)
                            {
                                Coordinates coords = currentStreet.getCoordinatesList().get(i);
                                if (ascending)
                                {
                                    if (coords.getX() > currentStop.getCoordinates().getX()) // the point on the street is after the stop
                                        Path.add(coords);
                                }
                                else
                                {
                                    if (coords.getX() < currentStop.getCoordinates().getX()) // the point on the street is after the stop
                                        Path.add(coords);
                                }
                            }
                        }
                        else // vertical street
                        {
                            ascending = currentStreet.getEnd().diffY(currentStreet.getBeginning()) > 0 ? true : false;
                            // TODO - the fors are similar, maybe it could be done using 1 while
                            for (int i = currentStreet.getCoordinatesList().size() - 2; i >= 0; --i)
                            {
                                Coordinates coords = currentStreet.getCoordinatesList().get(i);
                                if (ascending)
                                {
                                    if (coords.getY() > currentStop.getCoordinates().getY()) // the point on the street is after the stop
                                        Path.add(coords);
                                }
                                else
                                {
                                    if (coords.getY() < currentStop.getCoordinates().getY()) // the point on the street is after the stop
                                        Path.add(coords);
                                }
                            }
                        }
                        Path.add(currentStop.getCoordinates());
                    }
                    else // this street has no stops
                    // add the coordinates of the whole street (except the meeting point with last street) to the path
                    {
                        for (int i = currentStreet.getCoordinatesList().size() - 2; i >= 0; --i)
                        {
                            Path.add(currentStreet.getCoordinatesList().get(i));
                        }
                    }
                }
            }
            lastStreet = currentStreet;
            lastStop = currentStop;
        }

        currentStop = RouteStartToEnd ? Route.get(0).getValue() : Route.get(Route.size() - 1).getValue();
        Position = Coordinates.create(currentStop.getCoordinates().getX(), currentStop.getCoordinates().getY());
        CurrentStreet = RouteStartToEnd ? Route.get(0).getKey() : Route.get(Route.size() - 1).getKey();
        // CurrentStreetIndex = RouteStartToEnd ? 0 : Route.size() - 1;
        NextPositionIndex = RouteStartToEnd ? 1 : (Path.size() - 2);
        NextStopIndex = RouteStartToEnd ? 1 : (Route.size() - 2);
        LastStop = currentStop;
    }

    /**
     * Set direction of the tram so it goes from first stop to last.
     */
    public void setDirectionStartEnd()
    {
        RouteStartToEnd = true;
    }

    /**
     * Set direction of the tram so it goes from last stop to first.
     */
    public void setDirectionEndStart()
    {
        RouteStartToEnd = false;
    }

    /**
     * Returns name of the tram.
     * @return - tram position
     */
    public String getName()
    {
        return Name;
    }

    /**
     * Returns route of the tram.
     * @return - trams route
     */
    public List<SimpleImmutableEntry<Street,Stop>> getRoute()
    {
        return Route;
    }

    /**
     * Returns path of the tram.
     * @return - trams path
     */
    public List<Coordinates> getPath()
    {
        return Path;
    }

    /**
     * Returns current position of the tram.
     * @return - tram position
     */
    public Coordinates getPosition()
    {
        return Position;
    }

    /**
     * Returns the direction of tram on its route.
     * @return - true if tram goes from start to end on it route, false otherwise
     */
    public boolean getDirection()
    {
        return RouteStartToEnd;
    }

    /**
     * Returns current delay.
     * @return - delay of tram
     */
    public int getDelay()
    {
        return Delay;
    }

    /**
     * Returns timetable of the tram.
     * @return - timetable of tram
     */
    public ArrayList<String> getTimetable()
    {
        return Timetable;
    }

    /**
     * Returns stop last visited by the tram.
     * @return - last visited stop
     */
    public Stop getLastStop()
    {
        return LastStop;
    }

    /**
     * Checks if the tram is still traveling.
     * @return - true if tram is is still traveling, false if it ended its ride
     */
    public boolean isTraveling()
    {
        return Traveling;
    }

    /**
     * Counts new position of the tram. The counted position is where the tram will be after specified amount of seconds pass.
     * @param seconds - specified seconds
     */
    public void move(int seconds)
    {
        int previousTime = seconds;
        while (seconds > 0)
        {
            if (Waiting > 0)
            {
                Waiting -= seconds;
                seconds = Waiting < 0 ? 0 - Waiting : 0;
                TravelTime += previousTime - seconds;
                previousTime = seconds;
                if (Waiting <= 0)
                {
                    int delay = TravelTime - DepartureTimes.get(NextDepartureIndex++);
                    Delay = delay > 0 ? delay / 60 : 0; // delete "/ 60" to show delay in seconds
                }
                if (Waiting >= 0) // it shouldn't travel yet
                    return;
                else // waiting ended and the tram should travel
                    Waiting = 0;
            }

            if (NextStopIndex > Route.size() - 1 || NextStopIndex < 0) // the tram has no more stops to go to
            {
                Traveling = false;
                return;
            }

            // travel until arrival at the closest point of interest or there is no more distance to be traveled in this move
            int distanceToTravel = CurrentStreet.getSpeed() * seconds;
            // int distanceToTravel = Route.get(CurrentStreetIndex).getKey().getSpeed() * seconds;
            boolean ascending;
            int distanceToNextPOI;
            if (Position.diffX(Path.get(NextPositionIndex)) != 0) // horizontal part of the street
            {
                ascending = Position.diffX(Path.get(NextPositionIndex)) < 0 ? true : false;
                distanceToNextPOI = ascending ? Path.get(NextPositionIndex).diffX(Position) : Position.diffX(Path.get(NextPositionIndex));
                if (distanceToNextPOI < distanceToTravel) // arrived at point of interest and there is still time left for traveling
                    Position.setX(Path.get(NextPositionIndex).getX());
                else // all time spent traveling with a chance of arriving right at a point of interest
                    Position.setX(ascending ? Position.getX() + distanceToTravel : Position.getX() - distanceToTravel);
            }
            else // vertical part of the street
            {
                ascending = Position.diffY(Path.get(NextPositionIndex)) < 0 ? true : false;
                distanceToNextPOI = ascending ? Path.get(NextPositionIndex).diffY(Position) : Position.diffY(Path.get(NextPositionIndex));
                if (distanceToNextPOI < distanceToTravel) // arrived at point of interest and there is still time left
                    Position.setY(Path.get(NextPositionIndex).getY());
                else // all time spent traveling with a chance of arriving right at a point of interest
                    Position.setY(ascending ? Position.getY() + distanceToTravel : Position.getY() - distanceToTravel);
            }

            if (distanceToNextPOI >= distanceToTravel)
                seconds = 0;
            else
                seconds -= (distanceToNextPOI / CurrentStreet.getSpeed()) + 1; // due to integer calculations, up to almost a second of travel time can be lost (on each crossroad or stop), so travel time is extended by 1 second, which is better as the tram should slow down before crossroad or stop anyway
                // seconds -= (distanceToNextPOI / Route.get(CurrentStreetIndex).getKey().getSpeed()) + 1; // due to integer calculations, up to almost a second of travel time can be lost (on each crossroad or stop), so travel time is extended by 1 second, which is better as the tram should slow down before crossroad or stop anyway

            TravelTime += previousTime - seconds;
            previousTime = seconds;

            if (Position.equals(Path.get(NextPositionIndex))) // tram arrived to a point of interest
            {
                NextPositionIndex = RouteStartToEnd ? NextPositionIndex + 1 : NextPositionIndex - 1;
                if (Position.equals(Route.get(NextStopIndex).getValue().getCoordinates())) // tram arrived to a stop
                {
                    LastStop = Route.get(NextStopIndex).getValue();
                    // CurrentStreetIndex = NextStopIndex;
                    Waiting = TravelTime + WaitingTime < DepartureTimes.get(NextDepartureIndex) ? DepartureTimes.get(NextDepartureIndex) - TravelTime : WaitingTime;
                    // find next stop that isn't null (if there are no more stops, leave it at the routes out of bounds index)
                    do
                        NextStopIndex = RouteStartToEnd ? NextStopIndex + 1 : NextStopIndex - 1;
                    while (NextStopIndex < Route.size() && NextStopIndex > -1 && Route.get(NextStopIndex).getValue() == null);
                }
                else // tram arrived to a poi other than a stop (end of street, crossroads, turn)
                {
                    // check if tram is on start of a detour, add detour delay to the trams delay if so
                    for (SimpleEntry<List<Coordinates>,Integer> detour : Detours)
                    {
                        if (detour.getKey().get(0).equals(Position) && detour.getKey().get(1).equals(Path.get(NextPositionIndex)) ||
                            detour.getKey().get(detour.getKey().size() - 1).equals(Position) && detour.getKey().get(detour.getKey().size() - 2).equals(Path.get(NextPositionIndex))
                        ) // last poi in a path must be a stop, and this point isn't a stop - no need to check if NextPositionIndex is out of bounds
                        {
                            Delay += detour.getValue();
                            break;
                        }
                    }

                    if (Position.equals(CurrentStreet.getBeginning()) || Position.equals(CurrentStreet.getEnd())) // tram arrived to an end of street
                        CurrentStreet = Route.get(NextStopIndex).getKey(); // TODO - next stop is expected to be on the next street which doesn't work for detour and in case there is no stop on next street

                    // if the trams next point of interest is on a different street (it is on a crossroads with the street), change current street
                    // boolean nextPOIOnCurrentStreet = false;
                    // for (Coordinates coords : Route.get(CurrentStreetIndex).getKey().getCoordinatesList())
                    // {
                    //     if (Path.get(NextPositionIndex).equals(coords))
                    //     {
                    //         nextPOIOnCurrentStreet = true;
                    //         break;
                    //     }
                    // }
                    // if (!nextPOIOnCurrentStreet &&
                    //     Path.get(NextPositionIndex).equals(Route.get(NextStopIndex).getValue().getCoordinates()) &&
                    //     Route.get(NextStopIndex).getKey() != Route.get(CurrentStreetIndex).getKey()
                    // ) // next poi is not in list of coordinates of current street and the next poi is not a stop on current street
                    //     CurrentStreetIndex = RouteStartToEnd ? CurrentStreetIndex + 1 : CurrentStreetIndex - 1;
                }
            }
        }
    }

    /**
     * Sets alternative path around a closed part of a street if the trams path leeds through the part of a street.
     * @param alternativePath - list of coordinates specifying path from beginning to end of closed part of a street
     * @param delay - delay that this detour causes
     */
    public void setDetour(List<Coordinates> alternativePath, int delay) // TODO - add List<Street> streetsOfAltPath
    // works if trams path doesn't go through the same point more than once, stop located on the end of the closed part of the street is deleted // TODO
    {
        // change the path if needed
        int indexOfBeggining = -1;
        int indexOfEnd = -1;
        for (int i = 0; i < Path.size(); ++i)
        {
            if (Path.get(i).equals(alternativePath.get(0)))
                indexOfBeggining = i;
            if (Path.get(i).equals(alternativePath.get(alternativePath.size() - 1)))
                indexOfEnd = i;
        }
        if (indexOfBeggining != -1 && indexOfEnd != -1) // trams path involves closed street part
        {
            if (!(Position.getX() > alternativePath.get(0).getX() && Position.getX() > alternativePath.get(alternativePath.size() - 1).getX()) &&
                !(Position.getX() < alternativePath.get(0).getX() && Position.getX() < alternativePath.get(alternativePath.size() - 1).getX()) &&
                !(Position.getY() > alternativePath.get(0).getY() && Position.getY() > alternativePath.get(alternativePath.size() - 1).getY()) &&
                !(Position.getY() < alternativePath.get(0).getY() && Position.getY() < alternativePath.get(alternativePath.size() - 1).getY())
            ) // if the tram is already on the street part that is closed, don't set the detour and let it continue out of the street part
                return;
            else // tram is not on the closed street part, set the detour
            {
                Detours.add(new SimpleEntry<List<Coordinates>,Integer>(alternativePath, delay));
                int poisToDelete = indexOfBeggining < indexOfEnd ? indexOfEnd - indexOfBeggining + 1 : indexOfBeggining - indexOfEnd + 1;
                int detourBeginning = indexOfBeggining < indexOfEnd ? indexOfBeggining : indexOfEnd;
                while (poisToDelete-- > 0)
                    Path.remove(detourBeginning);
                if (indexOfBeggining < indexOfEnd)
                    for (int i = 0; i < alternativePath.size(); ++i)
                        Path.add(detourBeginning++, alternativePath.get(i));
                else
                    for (int i = alternativePath.size() - 1; i >= 0; --i)
                        Path.add(detourBeginning++, alternativePath.get(i));

                int poiIncrease = indexOfBeggining < indexOfEnd ? alternativePath.size() - (indexOfEnd - indexOfBeggining + 1) : alternativePath.size() - (indexOfBeggining - indexOfEnd + 1);
                if (NextPositionIndex >= indexOfBeggining && NextPositionIndex >= indexOfEnd)
                    NextPositionIndex += poiIncrease;

                // if a stop is between start and end of the detour, delete it
                int whichStopInOrder = 0;
                for (int i = 0; i < Route.size(); ++i)
                {
                    Stop stop = Route.get(i).getValue();
                    if (stop != null)
                    {
                        Coordinates stopCoords = stop.getCoordinates();
                        if (!(stopCoords.getX() > alternativePath.get(0).getX() && stopCoords.getX() > alternativePath.get(alternativePath.size() - 1).getX()) &&
                            !(stopCoords.getX() < alternativePath.get(0).getX() && stopCoords.getX() < alternativePath.get(alternativePath.size() - 1).getX()) &&
                            !(stopCoords.getY() > alternativePath.get(0).getY() && stopCoords.getY() > alternativePath.get(alternativePath.size() - 1).getY()) &&
                            !(stopCoords.getY() < alternativePath.get(0).getY() && stopCoords.getY() < alternativePath.get(alternativePath.size() - 1).getY())
                        )
                        {
                            if (i < NextStopIndex)
                                --NextStopIndex;
                            else if (i == NextStopIndex)
                            {
                                NextStopIndex = RouteStartToEnd ? NextStopIndex : NextStopIndex - 1;
                                while (NextStopIndex < Route.size() && NextStopIndex > -1 && Route.get(NextStopIndex).getValue() == null)
                                    NextStopIndex = RouteStartToEnd ? NextStopIndex + 1: NextStopIndex - 1;
                            }
                            Route.remove(i--);
                            Timetable.remove(whichStopInOrder);
                            DepartureTimes.remove(whichStopInOrder--);
                        }
                        ++whichStopInOrder;
                    }
                }
            }
        }
    }
}
