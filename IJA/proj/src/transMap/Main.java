// Project: IJA - Public transportation
// Authors: Michal Cibák    - xcibak00
//          Erika Fašánková - xfasan00
// Content: Main class of the program with main method.

package transMap;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Scene;
import javafx.scene.control.SplitPane;
import javafx.stage.Stage;

import java.io.FileInputStream;

// import java.io.IOException;
// import javafx.stage.StageStyle;

/**
* Represents the main class of aplication.
* It contains the main method.
*
* @author Michal Cibák
* @author Erika Fašánková
*/
public class Main extends Application
{
    @Override
    public void start(Stage stage) throws Exception // throws IOException
    {
        FXMLLoader loader = new FXMLLoader();
        FileInputStream fxmlStream = new FileInputStream("src/transMap/GUI.fxml");
        SplitPane root = (SplitPane) loader.load(fxmlStream);

        // stage.initStyle(StageStyle.DECORATED); // deafult
        stage.setTitle("Transportation Map v1.0");
        Scene scene = new Scene(root);
        stage.setScene(scene);

        stage.show();
    }

    public static void main(String[] args)
    {
        System.out.println("Main: Opening application window.");
        Application.launch(args);
        System.out.println("Main: Ending program.");
    }
}
