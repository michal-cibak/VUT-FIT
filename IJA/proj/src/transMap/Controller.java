// Project: IJA - Public transportation
// Authors: Michal Cibák    - xcibak00
//          Erika Fašánková - xfasan00
// Content: Class representing a controller of the program.

package transMap;

import javafx.application.Platform;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.event.EventHandler;
import javafx.fxml.FXML;
import javafx.geometry.VPos;
import javafx.scene.Group;
import javafx.scene.control.Label;
import javafx.scene.control.Slider;
import javafx.scene.control.TextArea;
import javafx.scene.input.MouseEvent;
import javafx.scene.input.ScrollEvent;
import javafx.scene.layout.Pane;
import javafx.scene.paint.Color;
import javafx.scene.shape.Circle;
import javafx.scene.shape.Line;
import javafx.scene.text.Text;

import java.io.File;
import java.io.FileNotFoundException;
import java.time.LocalTime;
import java.util.Scanner;
import java.util.Timer;
import java.util.TimerTask;
import java.util.List;
import java.util.ArrayList;
import java.util.Collections;
import java.util.AbstractMap.SimpleEntry;

/**
 * Controller class where GUI is handled.
 *
 * @author Michal Cibák
 * @author Erika Fašánková
 */
public class Controller
{
    @FXML
    private Pane LeftPanel;
    @FXML
    private Pane MapPane;
    @FXML
    private TextArea TimetableArea;
    @FXML
    private Slider SpeedSlider;
    @FXML
    private Slider TrafficSlider;
    @FXML
    private Text ClockLabel;
    @FXML
    private Label DelayLabel;
    @FXML
    private TextArea DelayArea;
    @FXML
    private TextArea setTimeArea;

    private boolean initialized = false;
    private Map CityMap;
    private Group Trams;
    private Timer MoveTimer;
    private int TimeMultiplier = 1; // should correspond to seconds (timer runs each second)
    private int Traffic = 1;
    private int AlternativeRouteDelay = 0;
    private LocalTime CurrentTime;
    private LocalTime PreviousTime;
    private Group ColorLines;
    private Tram ClickedTram;
    private List<SimpleEntry<SimpleEntry<List<Line>,List<Coordinates>>,Integer>> AlternativeRoutes;
    private SimpleEntry<SimpleEntry<List<Line>,List<Coordinates>>,Integer> AlternativeRoute;

    private ArrayList<ArrayList<String>> Orange1Timetable;
    private ArrayList<ArrayList<String>> Orange2Timetable;
    private ArrayList<ArrayList<String>> Blue1Timetable;
    private ArrayList<ArrayList<String>> Blue2Timetable;
    private ArrayList<ArrayList<String>> Green1Timetable;
    private ArrayList<ArrayList<String>> Green2Timetable;
    private ArrayList<ArrayList<String>> Purple1Timetable;
    private ArrayList<ArrayList<String>> Purple2Timetable;
    private ArrayList<ArrayList<String>> Red1Timetable;
    private ArrayList<ArrayList<String>> Red2Timetable;
    private String setTimeValue = "7:4:10";

    public Controller()
    {
        ;
    }

    @FXML
    private void initialize()
    {
        Orange1Timetable = new ArrayList<ArrayList<String>>();
        Orange2Timetable = new ArrayList<ArrayList<String>>();
        Blue1Timetable = new ArrayList<ArrayList<String>>();
        Blue2Timetable = new ArrayList<ArrayList<String>>();
        Green1Timetable = new ArrayList<ArrayList<String>>();
        Green2Timetable = new ArrayList<ArrayList<String>>();
        Purple1Timetable = new ArrayList<ArrayList<String>>();
        Purple2Timetable = new ArrayList<ArrayList<String>>();
        Red1Timetable = new ArrayList<ArrayList<String>>();
        Red2Timetable = new ArrayList<ArrayList<String>>();

        String[] timeValues = setTimeValue.split(":");
        if (timeValues.length == 2)
            CurrentTime = LocalTime.of(Integer.parseInt(timeValues[0]), Integer.parseInt(timeValues[1]));
        else if (timeValues.length == 3)
            CurrentTime = LocalTime.of(Integer.parseInt(timeValues[0]), Integer.parseInt(timeValues[1]), Integer.parseInt(timeValues[2]));
        else
            CurrentTime = LocalTime.of(6, 59, 55);

        ClockLabel.setText(CurrentTime.toString());
        PreviousTime = null;

        CityMap = new Map("Sim City");

        loadStreets();
        paintStreets();
        loadStops();
        paintStops();
        loadLines();
        loadTimetable();
        Trams = new Group();
        MapPane.getChildren().add(Trams);
        ClickedTram = null;
        AlternativeRoute = null;
        AlternativeRoutes = new ArrayList<>();
        ColorLines = null;
        loadTrams();
        paintTrams();

        if (!initialized)
        {
            addSliderListeners();
            initialized = true;
            startSimulation();
        }
    }

    @FXML
    private void zooming(ScrollEvent event)
    {
        event.consume();
        double zoomVal;

        if (event.getDeltaY() < 0)
            zoomVal = 0.95;
        else
            zoomVal = 1.05;

        MapPane.setScaleX(MapPane.getScaleX() * zoomVal);
        MapPane.setScaleY(MapPane.getScaleY() * zoomVal);

        MapPane.layout();
    }

    /**
     * Sets specified time in minutes as delay to be used for next detour that is made.
     */
    @FXML
    public void enterDelay()
    {
        String DelayAreaText = DelayArea.getText();
        if (DelayAreaText.equals(""))
        {
            DelayLabel.setText("Enter detour delay (" + AlternativeRouteDelay + " min)");
        }
        else
        {
            try
            {
                AlternativeRouteDelay = Integer.parseInt(DelayAreaText);
            }
            catch (NumberFormatException exception)
            {
                DelayLabel.setText("Enter detour delay (" + AlternativeRouteDelay + " min)");
                return;
            }
            DelayLabel.setText("Detour delay: " + AlternativeRouteDelay + " min");
        }
        DelayArea.setText("");
    }

    @FXML
    private void resetApp()
    {
        MapPane.getChildren().clear();
        initialize();
        leftPanel(ClickedTram);
    }

    @FXML
    public void setTime()
    {
        setTimeValue = setTimeArea.getText();
        resetApp();
    }

    /**
     * Adds listeners to sliders (binds their values to variables).
     */
    private void addSliderListeners()
    {
        SpeedSlider.valueProperty().addListener(new ChangeListener<Number>()
        {
            @Override
            public void changed(ObservableValue<? extends Number> observableValue, Number oldValue, Number newValue)
            {
                TimeMultiplier = newValue.intValue();
            }
        });

        TrafficSlider.valueProperty().addListener(new ChangeListener<Number>()
        {
            @Override
            public void changed(ObservableValue<? extends Number> observableValue, Number oldValue, Number newValue)
            {
                Traffic = newValue.intValue();
            }
        });
    }

    /**
     * Adds current information about the tram into left panel.
     * @param tram - tram to take information from
     */
    private void leftPanel(Tram tram)
    {
        if (tram == null)
            TimetableArea.clear();
        else
        {
            String tramName = tram.getName();
            String timetableText = "";
            timetableText += (tramName + "\n");
            for (int i = 0; i < tramName.length(); i++)
                timetableText += ("-");
            timetableText += ("\nDelay: " + tram.getDelay() + " minutes\n");
            timetableText += ("Last Stop: " + tram.getLastStop().getName() + "\n");
            for (int i = 0; i < tramName.length(); i++)
                timetableText += ("-");
            if (tram.getDirection()) {
                int iterT = 0;
                for (int i = 0; i < tram.getRoute().size(); i++) {
                    Stop stop = tram.getRoute().get(i).getValue();
                    if (stop != null) {
                        timetableText += ("\n" + stop.getName());
                        if (iterT < tram.getTimetable().size()){
                            timetableText += (" : " + tram.getTimetable().get(iterT));
                        }
                    }
                    iterT++;
                }
            }
            else {
                int iterF = 0;
                for (int i = tram.getRoute().size() - 1; i >= 0; i--) {
                    Stop stop = tram.getRoute().get(i).getValue();
                    if (stop != null) {
                        timetableText += ("\n" + stop.getName());
                        if (iterF < tram.getTimetable().size()) {
                            timetableText += (" : " + tram.getTimetable().get(iterF));
                        }
                    }
                    iterF++;
                }
            }
            if (!TimetableArea.getText().equals(timetableText))
                TimetableArea.setText(timetableText);
        }
    }

    /**
     * Highlights path of the tram.
     * @param tram - tram to take path from
     */
    private void highlightPath(Tram tram)
    {
        if (ColorLines != null)
        {
            ColorLines.getChildren().clear();
            ColorLines = null;
        }
        else
        {
            ColorLines = new Group();
            int lenPath = tram.getPath().size();
            int i = 1;

            while (i < lenPath)
            {
                Line line = new Line(tram.getPath().get(i - 1).getX(), tram.getPath().get(i - 1).getY(), tram.getPath().get(i).getX(), tram.getPath().get(i).getY());

                if (tram.getName().startsWith("Orange"))
                    line.setStroke(Color.ORANGE);
                else if (tram.getName().startsWith("Blue"))
                    line.setStroke(Color.BLUE);
                else if (tram.getName().startsWith("Green"))
                    line.setStroke(Color.GREEN);
                else if (tram.getName().startsWith("Purple"))
                    line.setStroke(Color.PURPLE);
                else if (tram.getName().startsWith("Red"))
                    line.setStroke(Color.CRIMSON);

                line.setStrokeWidth(5);
                ColorLines.getChildren().add(line);
                i++;
            }
            MapPane.getChildren().add(ColorLines);
        }
    }

    /**
     * Loads streets from a file.
     */
    private void loadStreets()
    {
        try
        {
            File newFile = new File("data/streets.txt");
            Scanner myReader = new Scanner(newFile);

            while (myReader.hasNextLine())
            {
                String data = myReader.nextLine();
                if (data.isEmpty())
                    continue;
                String[] splitData = data.split(":");
                int i = 1;
                int j = 2;
                List<Coordinates> splitDataCoord = new ArrayList<>();

                while (j < splitData.length)
                {
                    Coordinates coord = Coordinates.create(Integer.parseInt(splitData[i]), Integer.parseInt(splitData[j]));
                    splitDataCoord.add(coord);
                    i = i + 2;
                    j = j + 2;
                }

                Coordinates[] coordsArray = new Coordinates[splitDataCoord.size()];
                coordsArray = splitDataCoord.toArray(coordsArray);

                CityMap.addStreet(Street.create(splitData[0], coordsArray));
            }
            myReader.close();
        }
        catch (FileNotFoundException e)
        {
            System.out.println("An error occurred while loading streets.");
            e.printStackTrace();
        }
    }

    /**
     * Loads stops from a file.
     */
    private void loadStops()
    {
        try
        {
            File newFile = new File("data/stops.txt");
            Scanner myReader = new Scanner(newFile);
            Street theStreet = null;

            while (myReader.hasNextLine())
            {
                String data = myReader.nextLine();
                if (data.isEmpty())
                    continue;
                String[] splitData = data.split(":");

                // find the street object
                if (theStreet == null || !theStreet.getName().equals(splitData[0])) // it needs to be found (it isn't in the variable)
                {
                    for (Street street : CityMap.getStreets()) // find street with specified name
                        if (street.getName().equals(splitData[0]))
                        {
                            theStreet = street;
                            break;
                        }
                    if (theStreet != null && !theStreet.getName().equals(splitData[0])) // street wasn't found
                        theStreet = null;
                }

                // add the stop to the street
                if (theStreet != null) // a street with specified name exist
                    theStreet.addStop(new Stop(splitData[1], Coordinates.create(Integer.parseInt(splitData[2]), Integer.parseInt(splitData[3]))));
            }
            myReader.close();
        }
        catch (FileNotFoundException e)
        {
            System.out.println("An error occurred while loading stops.");
            e.printStackTrace();
        }
    }

    /**
     * Loads lines from a file.
     */
    private void loadLines()
    {
        try
        {
            File newFile = new File("data/lines.txt");
            Scanner myReader = new Scanner(newFile);
            transMap.Line line;
            Stop stop;

            while (myReader.hasNextLine())
            {
                String data = myReader.nextLine();
                if (data.isEmpty())
                    continue;
                String[] splitName = data.split("=");

                line = new transMap.Line(splitName[0]);

                String[] splitData = splitName[1].split(":");

                for (String stopName : splitData)
                    if ((stop = CityMap.getStopByName(stopName)) != null)
                        line.addStop(stop);
                CityMap.addLine(line);
            }
            myReader.close();
        }
        catch (FileNotFoundException e)
        {
            System.out.println("An error occurred.");
            e.printStackTrace();
        }
    }

    /**
    * Loads timetable.
    */
    private void loadTimetable()
    {
        try
        {
            File newFile = new File("data/timetable.txt");
            Scanner myReader = new Scanner(newFile);

            while (myReader.hasNextLine())
            {
                String data = myReader.nextLine();
                if (data.isEmpty())
                    continue;
                String[] splitName = data.split("=");
                String[] splitData = splitName[1].split(":");
                ArrayList<String> timeData = new ArrayList<String>();

                if (splitName[0].equals("Orange 1")) {
                    for (String time : splitData) {
                        timeData.add(time);
                    }
                    Orange1Timetable.add(timeData);
                }
                else if (splitName[0].equals("Orange 2")) {
                    for (String time : splitData) {
                        timeData.add(time);
                    }
                    Orange2Timetable.add(timeData);
                }
                else if (splitName[0].equals("Blue 1")) {
                    for (String time : splitData) {
                        timeData.add(time);
                    }
                    Blue1Timetable.add(timeData);
                }
                else if (splitName[0].equals("Blue 2")) {
                    for (String time : splitData) {
                        timeData.add(time);
                    }
                    Blue2Timetable.add(timeData);
                }
                else if (splitName[0].equals("Green 1")) {
                    for (String time : splitData) {
                        timeData.add(time);
                    }
                    Green1Timetable.add(timeData);
                }
                else if (splitName[0].equals("Green 2")) {
                    for (String time : splitData) {
                        timeData.add(time);
                    }
                    Green2Timetable.add(timeData);
                }
                else if (splitName[0].equals("Purple 1")) {
                    for (String time : splitData) {
                        timeData.add(time);
                    }
                    Purple1Timetable.add(timeData);
                }
                else if (splitName[0].equals("Purple 2")) {
                    for (String time : splitData) {
                        timeData.add(time);
                    }
                    Purple2Timetable.add(timeData);
                }
                else if (splitName[0].equals("Red 1")) {
                    for (String time : splitData) {
                        timeData.add(time);
                    }
                    Red1Timetable.add(timeData);
                }
                else if (splitName[0].equals("Red 2")) {
                    for (String time : splitData) {
                        timeData.add(time);
                    }
                    Red2Timetable.add(timeData);
                }
            }
            myReader.close();
        }
        catch (FileNotFoundException e)
        {
            System.out.println("An error occurred.");
            e.printStackTrace();
        }

        // basic check of loaded times
        for (ArrayList<String> departures : Orange1Timetable)
        {
            if (CityMap.getLineByName("Orange Line").getRoute().size() != departures.size())
                System.out.println("Wrong amount of times in timetable 1 for Orange Line, first time is " + departures.get(0));
        }

        for (ArrayList<String> departures : Orange2Timetable)
        {
            if (CityMap.getLineByName("Orange Line").getRoute().size() != departures.size())
                System.out.println("Wrong amount of times in timetable 2 for Orange Line, first time is " + departures.get(0));
        }

        for (ArrayList<String> departures : Blue1Timetable)
        {
            if (CityMap.getLineByName("Blue Line").getRoute().size() != departures.size())
                System.out.println("Wrong amount of times in timetable 1 for Blue Line, first time is " + departures.get(0));
        }

        for (ArrayList<String> departures : Blue2Timetable)
        {
            if (CityMap.getLineByName("Blue Line").getRoute().size() != departures.size())
                System.out.println("Wrong amount of times in timetable 2 for Blue Line, first time is " + departures.get(0));
        }

        for (ArrayList<String> departures : Green1Timetable)
        {
            if (CityMap.getLineByName("Green Line").getRoute().size() != departures.size())
                System.out.println("Wrong amount of times in timetable 1 for Green Line, first time is " + departures.get(0));
        }

        for (ArrayList<String> departures : Green2Timetable)
        {
            if (CityMap.getLineByName("Green Line").getRoute().size() != departures.size())
                System.out.println("Wrong amount of times in timetable 2 for Green Line, first time is " + departures.get(0));
        }

        for (ArrayList<String> departures : Purple1Timetable)
        {
            if (CityMap.getLineByName("Purple Line").getRoute().size() != departures.size())
                System.out.println("Wrong amount of times in timetable 1 for Purple Line, first time is " + departures.get(0));
        }

        for (ArrayList<String> departures : Purple2Timetable)
        {
            if (CityMap.getLineByName("Purple Line").getRoute().size() != departures.size())
                System.out.println("Wrong amount of times in timetable 2 for Purple Line, first time is " + departures.get(0));
        }

        for (ArrayList<String> departures : Red1Timetable)
        {
            if (CityMap.getLineByName("Red Line").getRoute().size() != departures.size())
                System.out.println("Wrong amount of times in timetable 1 for Red Line, first time is " + departures.get(0));
        }

        for (ArrayList<String> departures : Red2Timetable)
        {
            if (CityMap.getLineByName("Red Line").getRoute().size() != departures.size())
                System.out.println("Wrong amount of times in timetable 2 for Red Line, first time is " + departures.get(0));
        }
    }

    /**
     * Loads several trams.
     */
    private void loadTrams()
    {
        for (int i = 0; i < Orange1Timetable.size(); ++i) // for each timetable entry (1 tram)
        {
            ArrayList<String> tramDepartureTimes = Orange1Timetable.get(i);

            // get the first time (when a tram should be generated)
            String startValue = tramDepartureTimes.get(0);
            String[] splitStartValue = startValue.split("\\.");
            LocalTime tramGenTime = LocalTime.of(Integer.parseInt(splitStartValue[0]), Integer.parseInt(splitStartValue[1]));

            // get the last time (when a tram should end its ride)
            String endValue = tramDepartureTimes.get(tramDepartureTimes.size() - 1);
            String[] splitEndValue = endValue.split("\\.");
            LocalTime tramEndTime = LocalTime.of(Integer.parseInt(splitEndValue[0]), Integer.parseInt(splitEndValue[1]));

            // if tram should be traveling now, generate it and simulate the time since its departure
            if (CurrentTime.isAfter(tramGenTime) && CurrentTime.isBefore(tramEndTime) || CurrentTime.equals(tramGenTime))
            {
                Tram tram = new Tram("Orange 1", CityMap.getLineByName("Orange Line").getRoute(), true, tramDepartureTimes);
                CityMap.addTram(tram);
                setAllAlternativeRoutesToTram(tram);
                tram.move(CurrentTime.toSecondOfDay() - tramGenTime.toSecondOfDay());
            }
        }

        for (int i = 0; i < Orange2Timetable.size(); ++i) // for each timetable entry (1 tram)
        {
            ArrayList<String> tramDepartureTimes = Orange2Timetable.get(i);

            // get the first time (when a tram should be generated)
            String value = tramDepartureTimes.get(0);
            String[] splitValue = value.split("\\.");
            LocalTime tramGenTime = LocalTime.of(Integer.parseInt(splitValue[0]), Integer.parseInt(splitValue[1]));

            // get the last time (when a tram should end its ride)
            String endValue = tramDepartureTimes.get(tramDepartureTimes.size() - 1);
            String[] splitEndValue = endValue.split("\\.");
            LocalTime tramEndTime = LocalTime.of(Integer.parseInt(splitEndValue[0]), Integer.parseInt(splitEndValue[1]));
            if (CurrentTime.isAfter(tramGenTime) && CurrentTime.isBefore(tramEndTime) || CurrentTime.equals(tramGenTime))
            {
                Tram tram = new Tram("Orange 2", CityMap.getLineByName("Orange Line").getRoute(), false, tramDepartureTimes);
                CityMap.addTram(tram);
                setAllAlternativeRoutesToTram(tram);
                tram.move(CurrentTime.toSecondOfDay() - tramGenTime.toSecondOfDay());
            }
        }

        for (int i = 0; i < Blue1Timetable.size(); ++i) // for each timetable entry (1 tram)
        {
            ArrayList<String> tramDepartureTimes = Blue1Timetable.get(i);

            // get the first time (when a tram should be generated)
            String value = tramDepartureTimes.get(0);
            String[] splitValue = value.split("\\.");
            LocalTime tramGenTime = LocalTime.of(Integer.parseInt(splitValue[0]), Integer.parseInt(splitValue[1]));

            // get the last time (when a tram should end its ride)
            String endValue = tramDepartureTimes.get(tramDepartureTimes.size() - 1);
            String[] splitEndValue = endValue.split("\\.");
            LocalTime tramEndTime = LocalTime.of(Integer.parseInt(splitEndValue[0]), Integer.parseInt(splitEndValue[1]));
            if (CurrentTime.isAfter(tramGenTime) && CurrentTime.isBefore(tramEndTime) || CurrentTime.equals(tramGenTime))
            {
                Tram tram = new Tram("Blue 1", CityMap.getLineByName("Blue Line").getRoute(), true, tramDepartureTimes);
                CityMap.addTram(tram);
                setAllAlternativeRoutesToTram(tram);
                tram.move(CurrentTime.toSecondOfDay() - tramGenTime.toSecondOfDay());
            }
        }

        for (int i = 0; i < Blue2Timetable.size(); ++i) // for each timetable entry (1 tram)
        {
            ArrayList<String> tramDepartureTimes = Blue2Timetable.get(i);

            // get the first time (when a tram should be generated)
            String value = tramDepartureTimes.get(0);
            String[] splitValue = value.split("\\.");
            LocalTime tramGenTime = LocalTime.of(Integer.parseInt(splitValue[0]), Integer.parseInt(splitValue[1]));

            // get the last time (when a tram should end its ride)
            String endValue = tramDepartureTimes.get(tramDepartureTimes.size() - 1);
            String[] splitEndValue = endValue.split("\\.");
            LocalTime tramEndTime = LocalTime.of(Integer.parseInt(splitEndValue[0]), Integer.parseInt(splitEndValue[1]));
            if (CurrentTime.isAfter(tramGenTime) && CurrentTime.isBefore(tramEndTime) || CurrentTime.equals(tramGenTime))
            {
                Tram tram = new Tram("Blue 2", CityMap.getLineByName("Blue Line").getRoute(), false, tramDepartureTimes);
                CityMap.addTram(tram);
                setAllAlternativeRoutesToTram(tram);
                tram.move(CurrentTime.toSecondOfDay() - tramGenTime.toSecondOfDay());
            }
        }

        for (int i = 0; i < Green1Timetable.size(); ++i) // for each timetable entry (1 tram)
        {
            ArrayList<String> tramDepartureTimes = Green1Timetable.get(i);

            // get the first time (when a tram should be generated)
            String value = tramDepartureTimes.get(0);
            String[] splitValue = value.split("\\.");
            LocalTime tramGenTime = LocalTime.of(Integer.parseInt(splitValue[0]), Integer.parseInt(splitValue[1]));

            // get the last time (when a tram should end its ride)
            String endValue = tramDepartureTimes.get(tramDepartureTimes.size() - 1);
            String[] splitEndValue = endValue.split("\\.");
            LocalTime tramEndTime = LocalTime.of(Integer.parseInt(splitEndValue[0]), Integer.parseInt(splitEndValue[1]));
            if (CurrentTime.isAfter(tramGenTime) && CurrentTime.isBefore(tramEndTime) || CurrentTime.equals(tramGenTime))
            {
                Tram tram = new Tram("Green 1", CityMap.getLineByName("Green Line").getRoute(), true, tramDepartureTimes);
                CityMap.addTram(tram);
                setAllAlternativeRoutesToTram(tram);
                tram.move(CurrentTime.toSecondOfDay() - tramGenTime.toSecondOfDay());
            }
        }

        for (int i = 0; i < Green2Timetable.size(); ++i) // for each timetable entry (1 tram)
        {
            ArrayList<String> tramDepartureTimes = Green2Timetable.get(i);

            // get the first time (when a tram should be generated)
            String value = tramDepartureTimes.get(0);
            String[] splitValue = value.split("\\.");
            LocalTime tramGenTime = LocalTime.of(Integer.parseInt(splitValue[0]), Integer.parseInt(splitValue[1]));

            // get the last time (when a tram should end its ride)
            String endValue = tramDepartureTimes.get(tramDepartureTimes.size() - 1);
            String[] splitEndValue = endValue.split("\\.");
            LocalTime tramEndTime = LocalTime.of(Integer.parseInt(splitEndValue[0]), Integer.parseInt(splitEndValue[1]));
            if (CurrentTime.isAfter(tramGenTime) && CurrentTime.isBefore(tramEndTime) || CurrentTime.equals(tramGenTime))
            {
                Tram tram = new Tram("Green 2", CityMap.getLineByName("Green Line").getRoute(), false, tramDepartureTimes);
                CityMap.addTram(tram);
                setAllAlternativeRoutesToTram(tram);
                tram.move(CurrentTime.toSecondOfDay() - tramGenTime.toSecondOfDay());
            }
        }

        for (int i = 0; i < Purple1Timetable.size(); ++i) // for each timetable entry (1 tram)
        {
            ArrayList<String> tramDepartureTimes = Purple1Timetable.get(i);

            // get the first time (when a tram should be generated)
            String value = tramDepartureTimes.get(0);
            String[] splitValue = value.split("\\.");
            LocalTime tramGenTime = LocalTime.of(Integer.parseInt(splitValue[0]), Integer.parseInt(splitValue[1]));

            // get the last time (when a tram should end its ride)
            String endValue = tramDepartureTimes.get(tramDepartureTimes.size() - 1);
            String[] splitEndValue = endValue.split("\\.");
            LocalTime tramEndTime = LocalTime.of(Integer.parseInt(splitEndValue[0]), Integer.parseInt(splitEndValue[1]));
            if (CurrentTime.isAfter(tramGenTime) && CurrentTime.isBefore(tramEndTime) || CurrentTime.equals(tramGenTime))
            {
                Tram tram = new Tram("Purple 1", CityMap.getLineByName("Purple Line").getRoute(), true, tramDepartureTimes);
                CityMap.addTram(tram);
                setAllAlternativeRoutesToTram(tram);
                tram.move(CurrentTime.toSecondOfDay() - tramGenTime.toSecondOfDay());
            }
        }

        for (int i = 0; i < Purple2Timetable.size(); ++i) // for each timetable entry (1 tram)
        {
            ArrayList<String> tramDepartureTimes = Purple2Timetable.get(i);

            // get the first time (when a tram should be generated)
            String value = tramDepartureTimes.get(0);
            String[] splitValue = value.split("\\.");
            LocalTime tramGenTime = LocalTime.of(Integer.parseInt(splitValue[0]), Integer.parseInt(splitValue[1]));

            // get the last time (when a tram should end its ride)
            String endValue = tramDepartureTimes.get(tramDepartureTimes.size() - 1);
            String[] splitEndValue = endValue.split("\\.");
            LocalTime tramEndTime = LocalTime.of(Integer.parseInt(splitEndValue[0]), Integer.parseInt(splitEndValue[1]));
            if (CurrentTime.isAfter(tramGenTime) && CurrentTime.isBefore(tramEndTime) || CurrentTime.equals(tramGenTime))
            {
                Tram tram = new Tram("Purple 2", CityMap.getLineByName("Purple Line").getRoute(), false, tramDepartureTimes);
                CityMap.addTram(tram);
                setAllAlternativeRoutesToTram(tram);
                tram.move(CurrentTime.toSecondOfDay() - tramGenTime.toSecondOfDay());
            }
        }

        for (int i = 0; i < Red1Timetable.size(); ++i) // for each timetable entry (1 tram)
        {
            ArrayList<String> tramDepartureTimes = Red1Timetable.get(i);

            // get the first time (when a tram should be generated)
            String value = tramDepartureTimes.get(0);
            String[] splitValue = value.split("\\.");
            LocalTime tramGenTime = LocalTime.of(Integer.parseInt(splitValue[0]), Integer.parseInt(splitValue[1]));

            // get the last time (when a tram should end its ride)
            String endValue = tramDepartureTimes.get(tramDepartureTimes.size() - 1);
            String[] splitEndValue = endValue.split("\\.");
            LocalTime tramEndTime = LocalTime.of(Integer.parseInt(splitEndValue[0]), Integer.parseInt(splitEndValue[1]));
            if (CurrentTime.isAfter(tramGenTime) && CurrentTime.isBefore(tramEndTime) || CurrentTime.equals(tramGenTime))
            {
                Tram tram = new Tram("Red 1", CityMap.getLineByName("Red Line").getRoute(), true, tramDepartureTimes);
                CityMap.addTram(tram);
                setAllAlternativeRoutesToTram(tram);
                tram.move(CurrentTime.toSecondOfDay() - tramGenTime.toSecondOfDay());
            }
        }

        for (int i = 0; i < Red2Timetable.size(); ++i) // for each timetable entry (1 tram)
        {
            ArrayList<String> tramDepartureTimes = Red2Timetable.get(i);

            // get the first time (when a tram should be generated)
            String value = tramDepartureTimes.get(0);
            String[] splitValue = value.split("\\.");
            LocalTime tramGenTime = LocalTime.of(Integer.parseInt(splitValue[0]), Integer.parseInt(splitValue[1]));

            // get the last time (when a tram should end its ride)
            String endValue = tramDepartureTimes.get(tramDepartureTimes.size() - 1);
            String[] splitEndValue = endValue.split("\\.");
            LocalTime tramEndTime = LocalTime.of(Integer.parseInt(splitEndValue[0]), Integer.parseInt(splitEndValue[1]));
            if (CurrentTime.isAfter(tramGenTime) && CurrentTime.isBefore(tramEndTime) || CurrentTime.equals(tramGenTime))
            {
                Tram tram = new Tram("Red 2", CityMap.getLineByName("Red Line").getRoute(), false, tramDepartureTimes);
                CityMap.addTram(tram);
                setAllAlternativeRoutesToTram(tram);
                tram.move(CurrentTime.toSecondOfDay() - tramGenTime.toSecondOfDay());
            }
        }
    }

    /**
     * Paints streets.
     */
    private void paintStreets()
    {
        // for each street, paint its parts and name
        for (Street street : CityMap.getStreets())
        {
            // paint each part of the street
            Coordinates coordsPrevious;
            Coordinates coordsActual = street.getBeginning();

            for (int i = 1; i < street.getCoordinatesList().size(); ++i)
            {
                coordsPrevious = coordsActual;
                coordsActual = street.getCoordinatesList().get(i);

                Line line = new Line(coordsPrevious.getX(), coordsPrevious.getY(), coordsActual.getX(), coordsActual.getY());
                line.setStrokeWidth(3);

                line.addEventFilter(MouseEvent.MOUSE_CLICKED, new EventHandler<MouseEvent>()
                {
                    @Override
                    public void handle(MouseEvent mouseEvent)
                    {
                        Object eventSource = mouseEvent.getSource();
                        if (eventSource instanceof Line)
                        {
                            Line sourceLine = (Line)eventSource;

                            // alternate route building
                            Coordinates lineStart = Coordinates.create((int) sourceLine.getStartX(), (int) sourceLine.getStartY());
                            Coordinates lineEnd = Coordinates.create((int) sourceLine.getEndX(), (int) sourceLine.getEndY());
                            if (AlternativeRoute == null) // clicked on part of street to be closed
                            {
                                if (sourceLine.getStroke() == Color.RED) // the clicked line is closed part of a street
                                {
                                    // TODO - delete corresponding alternative route (open the street)
                                    return;
                                }
                                else if (sourceLine.getStroke() == Color.BLUE) // the clicked line is part of a alternative route
                                {
                                    // TODO - change already set alternative routes which are using this part of the street
                                    return;
                                }
                                else
                                {
                                    sourceLine.setStroke(Color.RED);
                                    AlternativeRoute = new SimpleEntry<>(new SimpleEntry<>(new ArrayList<Line>(), new ArrayList<Coordinates>()), 0);
                                    AlternativeRoute.getKey().getKey().add(sourceLine);
                                    AlternativeRoute.getKey().getValue().add(lineStart);
                                    AlternativeRoute.getKey().getValue().add(lineEnd);
                                    return;
                                }
                            }
                            else // clicked on part of street to be used as an alternate route for the closed part
                            {
                                if (sourceLine.equals(AlternativeRoute.getKey().getKey().get(0))) // the clicked line is the same as the closed street part
                                // scrap the alternative route making // TODO - for now, nothing happens
                                {
                                    // change color of colored thingies back to black, but remember not to uncolor line that is part of other alternative route!
                                    // AlternativeRoute = null;
                                    return;
                                }
                                else if (sourceLine.getStroke() != Color.RED) // the clicked line is not closed part of a street
                                {
                                    if (AlternativeRoute.getKey().getKey().size() == 1) // the only selected line so far was the part of the street to be closed
                                        if (lineStart.equals(AlternativeRoute.getKey().getValue().get(0)) || lineEnd.equals(AlternativeRoute.getKey().getValue().get(0))) // the clicked line is connected to first coordinate of closed street part
                                            Collections.swap(AlternativeRoute.getKey().getValue(), 0, 1);
                                    Coordinates lastCoords = AlternativeRoute.getKey().getValue().get(AlternativeRoute.getKey().getValue().size() - 1);
                                    if (lineStart.equals(lastCoords) || lineEnd.equals(lastCoords)) // the clicked line is connected to end of last clicked street part
                                    {
                                        sourceLine.setStroke(Color.BLUE);
                                        AlternativeRoute.getKey().getKey().add(sourceLine);
                                        Coordinates firstCoords = AlternativeRoute.getKey().getValue().get(0);
                                        if (lineStart.equals(firstCoords) || lineEnd.equals(firstCoords)) // the clicked line is the final line marking the alternative route
                                        {
                                            Collections.rotate(AlternativeRoute.getKey().getValue(), -1); // move first coordinates to end of the list
                                            AlternativeRoute.setValue(AlternativeRouteDelay);
                                            for (Tram tram : CityMap.getTrams())
                                                setAlternativeRouteToTram(AlternativeRoute, tram);
                                            AlternativeRoutes.add(AlternativeRoute);
                                            AlternativeRoute = null;
                                            return;
                                        }
                                        if (lineStart.equals(lastCoords)) // beginning of the clicked line is connected to the end of last clicked street part
                                            AlternativeRoute.getKey().getValue().add(lineEnd);
                                        else // end of the clicked line is connected to the end of last clicked street part
                                            AlternativeRoute.getKey().getValue().add(lineStart);
                                        return;
                                    }
                                    else // the clicked line is not connected to end of last clicked street part
                                        return;
                                }
                                else // the clicked line is closed part of a street
                                    return;
                            }
                        }
                        else
                        {
                            System.out.println("Clicked line isn't a line... ?");
                        }
                    }
                });

                MapPane.getChildren().add(line);

                // paint street name for each part of the street
                // Text text = new Text(street.getName());
                // text.setTextOrigin(VPos.CENTER);
                // text.setX((coordsPrevious.getX() + coordsActual.getX()) / 2 - text.getLayoutBounds().getWidth() / 2);
                // text.setY((coordsPrevious.getY() + coordsActual.getY()) / 2);
                // if ((coordsPrevious.getX() - coordsActual.getX()) == 0) // part of the street is vertical
                // {
                //     text.setRotate(90); // rotate the text
                //     text.setX(text.getX() + 10); // shift the text right
                // }
                // else // part of the street is horizontal
                //     text.setY(text.getY() - 10); // shift the text up
                // MapPane.getChildren().add(text);
            }

            // paint street name between beginning and end of the street (looks ok only if the street is a straight line (no turns))
            Text text = new Text(street.getName());

            text.addEventFilter(MouseEvent.MOUSE_CLICKED, new EventHandler<MouseEvent>()
            {
                @Override
                public void handle(MouseEvent mouseEvent)
                {
                    Object eventSource = mouseEvent.getSource();
                    if (eventSource instanceof Text)
                    {
                        Text text = (Text)eventSource;
                        String streetName = text.getText();
                        Street street = CityMap.getStreetByName(streetName);
                        if (Traffic <= 1)
                        {
                            street.setSpeed(3);
                            text.setFill(Color.BLACK);
                        }
                        else if (Traffic == 2)
                        {
                            street.setSpeed(2);
                            text.setFill(Color.BLUE);
                        }
                        else
                        {
                            street.setSpeed(1);
                            text.setFill(Color.RED);
                        }
                    }
                    else
                        System.out.println("Clicked text isn't a text... ?");
                }
            });

            text.setTextOrigin(VPos.CENTER);
            text.setX((street.getBeginning().getX() + street.getEnd().getX()) / 2 - text.getLayoutBounds().getWidth() / 2);
            text.setY((street.getBeginning().getY() + street.getEnd().getY()) / 2);
            if ((street.getBeginning().getX() - street.getEnd().getX()) == 0) // part of the street is vertical
            {
                text.setRotate(90); // rotate the text
                text.setX(text.getX() + 10); // shift the text right
            }
            else // part of the street is horizontal
                text.setY(text.getY() - 10); // shift the text up

            MapPane.getChildren().add(text);
        }
    }

    /**
     * Paints stops.
     * Shows stop name when hovering over the stop with mouse cursor.
     */
    private void paintStops()
    {
        for (Street street : CityMap.getStreets())
        {
            for (Stop stop : street.getStops())
            {
                Circle circle = new Circle(stop.getCoordinates().getX(), stop.getCoordinates().getY(), 6.0, Color.SLATEGRAY);
                MapPane.getChildren().add(circle);

                Text text = new Text((stop.getCoordinates().getX()), (stop.getCoordinates().getY()) - 10, stop.getName());
                text.setX(text.getX() - text.getLayoutBounds().getWidth() / 2);
                text.setFill(Color.SLATEGRAY);
                text.setVisible(false);
                MapPane.getChildren().add(text);

                circle.addEventFilter(MouseEvent.MOUSE_ENTERED, new EventHandler<MouseEvent>()
                {
                    @Override
                    public void handle(MouseEvent mouseEvent)
                    {
                        text.setVisible(true);
                    }
                });

                circle.addEventFilter(MouseEvent.MOUSE_EXITED, new EventHandler<MouseEvent>()
                {
                    @Override
                    public void handle(MouseEvent mouseEvent)
                    {
                        text.setVisible(false);
                    }
                });
            }
        }
    }

    /**
     * Paints specified tram.
     * Tram shows information about itself when clicked on.
     * @param tram - tram to be painted
     */
    private void paintTram(Tram tram)
    {
        Circle circle = new Circle(tram.getPosition().getX(), tram.getPosition().getY(), 10.0);
        if (tram.getName().startsWith("Orange"))
            circle.setFill(Color.ORANGE);
        else if (tram.getName().startsWith("Blue"))
            circle.setFill(Color.BLUE);
        else if (tram.getName().startsWith("Green"))
            circle.setFill(Color.GREEN);
        else if (tram.getName().startsWith("Purple"))
            circle.setFill(Color.PURPLE);
        else if (tram.getName().startsWith("Red"))
            circle.setFill(Color.CRIMSON);
        circle.addEventFilter(MouseEvent.MOUSE_CLICKED, new EventHandler<MouseEvent>()
        {
            @Override
            public void handle(MouseEvent mouseEvent)
            {
                ClickedTram = tram;
                highlightPath(tram);
                leftPanel(tram);
            }
        });
        Trams.getChildren().add(circle);
    }

    /**
     * Paints all initial trams.
     * Tram shows information about itself when clicked on.
     */
    private void paintTrams()
    {
        // MapPane.getChildren().add(Trams); // called in initialize function
        for (int index = 0; index < CityMap.getTrams().size(); ++index)
        {
            Tram tram = CityMap.getTrams().get(index);
            if (tram.isTraveling())
                paintTram(tram);
            else
                CityMap.getTrams().remove(index--);
        }
    }

    /**
     * Moves trams to their next position, removes them from the map if they end their ride.
     */
    private void moveTrams()
    {
        if (CityMap.getTrams().size() == Trams.getChildren().size()) // same amount of trams drawn as objects representing them (should always be true)
        {
            int index = 0;
            while (index < CityMap.getTrams().size())
            {
                Tram tram = CityMap.getTrams().get(index);
                tram.move(TimeMultiplier);
                if (tram.isTraveling())
                {
                    Circle circle = (Circle) Trams.getChildren().get(index);
                    circle.setCenterX(tram.getPosition().getX());
                    circle.setCenterY(tram.getPosition().getY());
                    ++index;
                }
                else
                {
                    if (tram == ClickedTram)
                        ClickedTram = null;
                    Trams.getChildren().remove(index);
                    CityMap.getTrams().remove(index);
                }
            }
        }
        else
            System.out.println("Error - different amount of trams drawn and objects representing them.");
    }

    /**
     * Makes tram use specified alternative route if the closed part of a street belongs to its route.
     * @param tram - tram for which the alterantive routes will be set
     */
    private void setAlternativeRouteToTram(SimpleEntry<SimpleEntry<List<Line>,List<Coordinates>>,Integer> altRoute, Tram tram)
    {
        tram.setDetour(altRoute.getKey().getValue(), altRoute.getValue());
    }

    /**
     * Makes tram use already specified alternative routes if the closed part of a street belongs to its route.
     * @param tram - tram for which the alterantive routes will be set
     */
    private void setAllAlternativeRoutesToTram(Tram tram)
    {
        for (SimpleEntry<SimpleEntry<List<Line>,List<Coordinates>>,Integer> altRoute : AlternativeRoutes)
            setAlternativeRouteToTram(altRoute, tram);
    }

    /**
     * Adds new trams according to timetable and current time.
     */
    private void generateTrams()
    {
        for (int i = 0; i < Orange1Timetable.size(); ++i) // for each timetable entry (1 tram)
        {
            // get the first time (when a tram should be generated)
            ArrayList<String> tramDepartureTimes = Orange1Timetable.get(i);
            String value = tramDepartureTimes.get(0);
            String[] splitValue = value.split("\\.");
            LocalTime tramGenTime = LocalTime.of(Integer.parseInt(splitValue[0]), Integer.parseInt(splitValue[1]));

            // if the time to generate a tram is now, generate the tram
            if (tramGenTime.equals(CurrentTime) || CurrentTime.isAfter(tramGenTime) && PreviousTime.isBefore(tramGenTime)) // the time to generate is just now or it was jumped over because of time multiplier
            {
                Tram tram = new Tram("Orange 1", CityMap.getLineByName("Orange Line").getRoute(), true, tramDepartureTimes);
                CityMap.addTram(tram);
                setAllAlternativeRoutesToTram(tram);
                tram.move(CurrentTime.toSecondOfDay() - tramGenTime.toSecondOfDay()); // if the tram should've been generated earlier, move it accordingly
                paintTram(tram);
            }
        }

        for (int i = 0; i < Orange2Timetable.size(); ++i) // for each timetable entry (1 tram)
        {
            // get the first time (when a tram should be generated)
            ArrayList<String> tramDepartureTimes = Orange2Timetable.get(i);
            String value = tramDepartureTimes.get(0);
            String[] splitValue = value.split("\\.");
            LocalTime tramGenTime = LocalTime.of(Integer.parseInt(splitValue[0]), Integer.parseInt(splitValue[1]));

            // if the time to generate a tram is now, generate the tram
            if (tramGenTime.equals(CurrentTime) || CurrentTime.isAfter(tramGenTime) && PreviousTime.isBefore(tramGenTime)) // the time to generate is just now or it was jumped over because of time multiplier
            {
                Tram tram = new Tram("Orange 2", CityMap.getLineByName("Orange Line").getRoute(), false, tramDepartureTimes);
                CityMap.addTram(tram);
                setAllAlternativeRoutesToTram(tram);
                tram.move(CurrentTime.toSecondOfDay() - tramGenTime.toSecondOfDay()); // if the tram should've been generated earlier, move it accordingly
                paintTram(tram);
            }
        }

        for (int i = 0; i < Blue1Timetable.size(); ++i) // for each timetable entry (1 tram)
        {
            // get the first time (when a tram should be generated)
            ArrayList<String> tramDepartureTimes = Blue1Timetable.get(i);
            String value = tramDepartureTimes.get(0);
            String[] splitValue = value.split("\\.");
            LocalTime tramGenTime = LocalTime.of(Integer.parseInt(splitValue[0]), Integer.parseInt(splitValue[1]));

            // if the time to generate a tram is now, generate the tram
            if (tramGenTime.equals(CurrentTime) || CurrentTime.isAfter(tramGenTime) && PreviousTime.isBefore(tramGenTime)) // the time to generate is just now or it was jumped over because of time multiplier
            {
                Tram tram = new Tram("Blue 1", CityMap.getLineByName("Blue Line").getRoute(), true, tramDepartureTimes);
                CityMap.addTram(tram);
                setAllAlternativeRoutesToTram(tram);
                tram.move(CurrentTime.toSecondOfDay() - tramGenTime.toSecondOfDay()); // if the tram should've been generated earlier, move it accordingly
                paintTram(tram);
            }
        }

        for (int i = 0; i < Blue2Timetable.size(); ++i) // for each timetable entry (1 tram)
        {
            // get the first time (when a tram should be generated)
            ArrayList<String> tramDepartureTimes = Blue2Timetable.get(i);
            String value = tramDepartureTimes.get(0);
            String[] splitValue = value.split("\\.");
            LocalTime tramGenTime = LocalTime.of(Integer.parseInt(splitValue[0]), Integer.parseInt(splitValue[1]));

            // if the time to generate a tram is now, generate the tram
            if (tramGenTime.equals(CurrentTime) || CurrentTime.isAfter(tramGenTime) && PreviousTime.isBefore(tramGenTime)) // the time to generate is just now or it was jumped over because of time multiplier
            {
                Tram tram = new Tram("Blue 2", CityMap.getLineByName("Blue Line").getRoute(), false, tramDepartureTimes);
                CityMap.addTram(tram);
                setAllAlternativeRoutesToTram(tram);
                tram.move(CurrentTime.toSecondOfDay() - tramGenTime.toSecondOfDay()); // if the tram should've been generated earlier, move it accordingly
                paintTram(tram);
            }
        }

        for (int i = 0; i < Green1Timetable.size(); ++i) // for each timetable entry (1 tram)
        {
            // get the first time (when a tram should be generated)
            ArrayList<String> tramDepartureTimes = Green1Timetable.get(i);
            String value = tramDepartureTimes.get(0);
            String[] splitValue = value.split("\\.");
            LocalTime tramGenTime = LocalTime.of(Integer.parseInt(splitValue[0]), Integer.parseInt(splitValue[1]));

            // if the time to generate a tram is now, generate the tram
            if (tramGenTime.equals(CurrentTime) || CurrentTime.isAfter(tramGenTime) && PreviousTime.isBefore(tramGenTime)) // the time to generate is just now or it was jumped over because of time multiplier
            {
                Tram tram = new Tram("Green 1", CityMap.getLineByName("Green Line").getRoute(), true, tramDepartureTimes);
                CityMap.addTram(tram);
                setAllAlternativeRoutesToTram(tram);
                tram.move(CurrentTime.toSecondOfDay() - tramGenTime.toSecondOfDay()); // if the tram should've been generated earlier, move it accordingly
                paintTram(tram);
            }
        }

        for (int i = 0; i < Green2Timetable.size(); ++i) // for each timetable entry (1 tram)
        {
            // get the first time (when a tram should be generated)
            ArrayList<String> tramDepartureTimes = Green2Timetable.get(i);
            String value = tramDepartureTimes.get(0);
            String[] splitValue = value.split("\\.");
            LocalTime tramGenTime = LocalTime.of(Integer.parseInt(splitValue[0]), Integer.parseInt(splitValue[1]));

            // if the time to generate a tram is now, generate the tram
            if (tramGenTime.equals(CurrentTime) || CurrentTime.isAfter(tramGenTime) && PreviousTime.isBefore(tramGenTime)) // the time to generate is just now or it was jumped over because of time multiplier
            {
                Tram tram = new Tram("Green 2", CityMap.getLineByName("Green Line").getRoute(), false, tramDepartureTimes);
                CityMap.addTram(tram);
                setAllAlternativeRoutesToTram(tram);
                tram.move(CurrentTime.toSecondOfDay() - tramGenTime.toSecondOfDay()); // if the tram should've been generated earlier, move it accordingly
                paintTram(tram);
            }
        }

        for (int i = 0; i < Purple1Timetable.size(); ++i) // for each timetable entry (1 tram)
        {
            // get the first time (when a tram should be generated)
            ArrayList<String> tramDepartureTimes = Purple1Timetable.get(i);
            String value = tramDepartureTimes.get(0);
            String[] splitValue = value.split("\\.");
            LocalTime tramGenTime = LocalTime.of(Integer.parseInt(splitValue[0]), Integer.parseInt(splitValue[1]));

            // if the time to generate a tram is now, generate the tram
            if (tramGenTime.equals(CurrentTime) || CurrentTime.isAfter(tramGenTime) && PreviousTime.isBefore(tramGenTime)) // the time to generate is just now or it was jumped over because of time multiplier
            {
                Tram tram = new Tram("Purple 1", CityMap.getLineByName("Purple Line").getRoute(), true, tramDepartureTimes);
                CityMap.addTram(tram);
                setAllAlternativeRoutesToTram(tram);
                tram.move(CurrentTime.toSecondOfDay() - tramGenTime.toSecondOfDay()); // if the tram should've been generated earlier, move it accordingly
                paintTram(tram);
            }
        }

        for (int i = 0; i < Purple2Timetable.size(); ++i) // for each timetable entry (1 tram)
        {
            // get the first time (when a tram should be generated)
            ArrayList<String> tramDepartureTimes = Purple2Timetable.get(i);
            String value = tramDepartureTimes.get(0);
            String[] splitValue = value.split("\\.");
            LocalTime tramGenTime = LocalTime.of(Integer.parseInt(splitValue[0]), Integer.parseInt(splitValue[1]));

            // if the time to generate a tram is now, generate the tram
            if (tramGenTime.equals(CurrentTime) || CurrentTime.isAfter(tramGenTime) && PreviousTime.isBefore(tramGenTime)) // the time to generate is just now or it was jumped over because of time multiplier
            {
                Tram tram = new Tram("Purple 2", CityMap.getLineByName("Purple Line").getRoute(), false, tramDepartureTimes);
                CityMap.addTram(tram);
                setAllAlternativeRoutesToTram(tram);
                tram.move(CurrentTime.toSecondOfDay() - tramGenTime.toSecondOfDay()); // if the tram should've been generated earlier, move it accordingly
                paintTram(tram);
            }
        }

        for (int i = 0; i < Red1Timetable.size(); ++i) // for each timetable entry (1 tram)
        {
            // get the first time (when a tram should be generated)
            ArrayList<String> tramDepartureTimes = Red1Timetable.get(i);
            String value = tramDepartureTimes.get(0);
            String[] splitValue = value.split("\\.");
            LocalTime tramGenTime = LocalTime.of(Integer.parseInt(splitValue[0]), Integer.parseInt(splitValue[1]));

            // if the time to generate a tram is now, generate the tram
            if (tramGenTime.equals(CurrentTime) || CurrentTime.isAfter(tramGenTime) && PreviousTime.isBefore(tramGenTime)) // the time to generate is just now or it was jumped over because of time multiplier
            {
                Tram tram = new Tram("Red 1", CityMap.getLineByName("Red Line").getRoute(), true, tramDepartureTimes);
                CityMap.addTram(tram);
                setAllAlternativeRoutesToTram(tram);
                tram.move(CurrentTime.toSecondOfDay() - tramGenTime.toSecondOfDay()); // if the tram should've been generated earlier, move it accordingly
                paintTram(tram);
            }
        }

        for (int i = 0; i < Red2Timetable.size(); ++i) // for each timetable entry (1 tram)
        {
            // get the first time (when a tram should be generated)
            ArrayList<String> tramDepartureTimes = Red2Timetable.get(i);
            String value = tramDepartureTimes.get(0);
            String[] splitValue = value.split("\\.");
            LocalTime tramGenTime = LocalTime.of(Integer.parseInt(splitValue[0]), Integer.parseInt(splitValue[1]));

            // if the time to generate a tram is now, generate the tram
            if (tramGenTime.equals(CurrentTime) || CurrentTime.isAfter(tramGenTime) && PreviousTime.isBefore(tramGenTime)) // the time to generate is just now or it was jumped over because of time multiplier
            {
                Tram tram = new Tram("Red 2", CityMap.getLineByName("Red Line").getRoute(), false, tramDepartureTimes);
                CityMap.addTram(tram);
                setAllAlternativeRoutesToTram(tram);
                tram.move(CurrentTime.toSecondOfDay() - tramGenTime.toSecondOfDay()); // if the tram should've been generated earlier, move it accordingly
                paintTram(tram);
            }
        }
    }

    /**
     * Starts a 1 second timer which controls clock, tram movements etc.
     */
    private void startSimulation()
    {
        TimerTask task = new TimerTask()
        {
            @Override
            public void run()
            {
                Platform.runLater(new Runnable()
                {
                    @Override
                    public void run()
                    {
                        moveTrams();
                        leftPanel(ClickedTram);
                        PreviousTime = CurrentTime.minusSeconds(0);
                        CurrentTime = CurrentTime.plusSeconds(TimeMultiplier);
                        ClockLabel.setText(CurrentTime.toString());
                        generateTrams();
                    }
                });
            }
        };
        MoveTimer = new Timer(true); // make it run as daemon thread so it doesn't block JVM from exiting
        MoveTimer.scheduleAtFixedRate(task, 1000, 1000);
    }
// END OF CLASS
}
