<?php
    require 'functions.php';

    print_header('Rezervace');

    // only 'pokladni' and above can access this page
    if (!has_rights_of('pokladni'))
    {
        print_footer();
        die();
    }

    if (array_key_exists('id', $_GET)) {
        try {
            $stmt = $pdo->prepare("SELECT * FROM Rezervace_navstevnika WHERE email = ?");
            $stmt->execute(array($_GET['id']));
            $uzivatel = $stmt->fetch();
            $stmt = $pdo->prepare("SELECT * FROM Festival WHERE id = ?");
            $stmt->execute(array($uzivatel['festival_id']));
            $festival = $stmt->fetch();
        }
        catch (PDOException $e) {
            echo '<h2>Spojení s databází selhalo: ' .$e->getMessage().'</h2>';
        }
        echo "<table>
            <tr>
            <th>ID uživatele</th><td>${uzivatel['email']}</td>
            </tr>
            <tr>
            <th>ID festivalu</th><td>${uzivatel['festival_id']}</td>
            </tr>
            <tr>";
        echo "<th>Aktuální stav</th><td>";
        if($_POST)
        {
            //aktualizace stavu navstevnika
            try {
                $stmt = $pdo->prepare("UPDATE Rezervace_navstevnika SET stav = ? WHERE email = ?");
                $stmt->execute(array($_POST['stav'], $uzivatel['email']));
            }
            catch (PDOException $e)
            {
                echo '<h2>Spojení s databází selhalo: ' .$e->getMessage().'</h2>';
            }
            if(isset($_POST))
            {
                echo "${_POST['stav']}</td>";
                //zadanka nezaplacena uzivatelova zadanka smazana z databaze, vstupenky znovu k dispozici
                if($_POST['stav']=='zamitnuto')
                {
                    try {
                        $stmt = $pdo->prepare("DELETE FROM Vstupenka Where rezervace_navstevnika_id = ?");
                        $stmt->execute(array($uzivatel['id']));
                        $pocet = $stmt->rowCount();
                        $kap = $festival['zaplneno'] - $pocet;
                        $stmt = $pdo->prepare("UPDATE Festival SET zaplneno=${kap} WHERE id = ?");
                        $stmt->execute(array($festival['id']));
                    }
                    catch (PDOException $e) {
                        echo '<h2>Spojení s databází selhalo: ' .$e->getMessage().'</h2>';
                    }
                }
            }
        }
        else {
            echo"${uzivatel['stav']}";
        }
        echo"</tr>
            </table>";

        echo "<form method='POST' action='Change_stateN.php?id=${uzivatel['email']}'>
            <select name='stav' id='stav'>
                <option value='uhrazeno'>uhrazeno</option>
                <option value='neuhrazeno'>neuhrazeno</option>
                <option value='zamitnuto'>zamitnuto</option>
            <input type='submit' name = 'submit' value='Odeslat'>
            </select></form>";
    }

    print_footer();
?>
