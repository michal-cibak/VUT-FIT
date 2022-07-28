<?php
    require 'functions.php';

    print_header('Rezervace');

    // only 'pokladni' and above can access this page
    if (!has_rights_of('pokladni'))
    {
        print_footer();
        die();
    }

    if (array_key_exists('reserv', $_GET)) {

        try {
            $reservN = $pdo->prepare("SELECT * FROM Rezervace_navstevnika WHERE festival_id = ?");
            $reservN->execute(array($_GET['reserv']));
            $reservU = $pdo->prepare("SELECT * FROM Rezervace_uzivatele WHERE festival_id = ?");
            $reservU->execute(array($_GET['reserv']));
        }
        catch (PDOException $e) {
            echo '<h2>Spojení s databází selhalo: ' .$e->getMessage().'</h2>';
        }
        echo "<h1>Neuhrazeno</h1>\n";
        echo "<table border='1'>\n";
        //neuhrazeni návštěvníci
        while ($reserv = $reservN->fetch()){
            if($reserv['stav']=="neuhrazeno") {
                echo "<tr><th>ID rezervace</th><td>${reserv['id']}</td><th>E-mail</th><td>${reserv['email']}</td><th>Stav</th><td>${reserv['stav']}</td><th>Návštěvník</th><td><a href=Change_stateN.php?id=${reserv['email']}><button>Změna stavu</button></a></td></tr>\n";
            }
        }
        //neuhrazení uživatelé
        while ($reserv = $reservU->fetch()){
            try {
                $stmt = $pdo->prepare("SELECT * FROM Uzivatel WHERE id = ?");
                $stmt->execute(array($reserv['uzivatel_id']));
                $uzivatel = $stmt->fetch();
            }
            catch (PDOException $e) {
                echo '<h2>Spojení s databází selhalo: ' .$e->getMessage().'</h2>';
            }
            if($reserv['stav']=="neuhrazeno") {
                echo "<tr><th>ID rezervace</th><td>${reserv['id']}</td><th>E-mail</th><td>${uzivatel['email']}</td><th>Stav</th><td>${reserv['stav']}</td><th>Uživatel</th><td><a href=Change_stateU.php?id=${reserv['id']}><button>Změna stavu</button></a></td>\n";
            }
        }
        echo "</table>";
        try {
            $reservN = $pdo->prepare("SELECT * FROM Rezervace_navstevnika WHERE festival_id = ?");
            $reservN->execute(array($_GET['reserv']));
            $reservU = $pdo->prepare("SELECT * FROM Rezervace_uzivatele WHERE festival_id = ?");
            $reservU->execute(array($_GET['reserv']));
        }
        catch (PDOException $e) {
            echo '<h2>Spojení s databází selhalo: ' .$e->getMessage().'</h2>';
        }
        //uhrazení návštěvníci
        echo "<h1>Uhrazeno</h1>\n";
        echo "<table border='2'>\n";
        while ($reserv = $reservN->fetch()){
            if($reserv['stav']=="uhrazeno") {
                echo "<tr><th>ID rezervace</th><td>${reserv['id']}</td><th>E-mail</th><td>${reserv['email']}</td><th>Stav</th><td>${reserv['stav']}</td><th>Návštěvník</th><td><a href=Change_stateN.php?id=${reserv['email']}><button>Změna stavu</button></a></td>";
            }
        }
        //uhrazení uživatelé
        while ($reserv = $reservU->fetch()){
            try {
                $stmt = $pdo->prepare("SELECT * FROM Uzivatel WHERE id = ?");
                $stmt->execute(array($reserv['uzivatel_id']));
                $uzivatel = $stmt->fetch();
            }
            catch (PDOException $e)
            {
                echo '<h2>Spojení s databází selhalo: ' .$e->getMessage().'</h2>';
            }
            if($reserv['stav']=="uhrazeno") {
                echo "<tr><th>ID rezervace</th><td>${reserv['id']}</td><th>E-mail</th><td>${uzivatel['email']}</td><th>Stav</th><td>${reserv['stav']}</td><th>Uživatel</th><td><a href=Change_stateU.php?id=${reserv['id']}><button>Změna stavu</button></a></td>";
            }
        }
        echo "</table>";
    }
?>
