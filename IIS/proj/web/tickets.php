<?php
    require 'functions.php';

    print_header('Vstupenky');

    // prisla pozadavka na zakoupeni vstupenky
    if (array_key_exists('buy', $_GET))
    {
        $stmt = $pdo->prepare('SELECT * FROM Festival WHERE id = ?');
        $stmt->execute(array($_GET['buy']));
        $festival = $stmt->fetch();

        $datum = date('Y-m-d');
        $datum_do = $festival['datum_do'];
        if ($datum_do < $datum)
        {
            echo "<h1>Festival již proběhl. Rezervovat vstupenky není možné.</h1>\n";
            print_footer();
            die();
        }

        if ($_POST) // V poli _POST něco je, odeslal se formulář
        {
            if (isset($_SESSION['user']))
            {
                $uzivatel = $pdo->query("SELECT id, email FROM Uzivatel WHERE prezdivka = '${_SESSION['user']}'")->fetch(); // safe to use - username is taken from database, so it can't cause issues
                $email = $uzivatel['email'];
            }
            else
                $email = $_POST['email'];

            $pocet_volnych = $festival['kapacita'] - $festival['zaplneno'];

            if ($email === '')
            {
                alert("Chybi email");
            }
            else if ($pocet_volnych < $_POST['vstupenky'])
            {
                alert("Tolik vstupenek uz neni k dispozici, mozno zakoupit pouze $pocet_volnych");
            }
            else
            {
                /*odeslani emailu uzivateli o tom ze rezervoval urcity pocet listku*/
                $hlavicka = 'From: skolabrnodtb@seznam.cz';
                $hlavicka .= "\nMIME-Version: 1.0\n";
                $hlavicka .= "Content-Type: text/html; charset=\"utf-8\"\n";
                $adresa = $email;
                $predmet = 'Vstupenky';
                $uspech = mb_send_mail($adresa, $predmet, "Bylo Vam rezervovano: " . $_POST['vstupenky'] . "vstupenek", $hlavicka);
                /*vstupenky pro uzivatele */
                if (isset($_SESSION['user'])) {
                    add_to_database(
                        array('uzivatel_id', 'festival_id'),
                        'Rezervace_uzivatele',
                        array($uzivatel['id'], $festival["id"]));
                    $last_id = $pdo->lastInsertId();
                    for ($i = 0; $i < $_POST['vstupenky']; $i++) {
                        add_to_database(
                            array('typ', 'rezervace_uzivatele_id'),
                            'Vstupenka',
                            array($_POST['vstupenka_typ'], $last_id));
                    }
                }
                /*vstupenky pro navstevniky*/
                else {
                    add_to_database(
                        array('email', 'festival_id'),
                        'Rezervace_navstevnika',
                        array($_POST['email'], $festival["id"]));
                    $last_id = $pdo->lastInsertId();
                    for ($i = 0; $i < $_POST['vstupenky']; $i++) {
                        add_to_database(
                            array('typ', 'rezervace_navstevnika_id'),
                            'Vstupenka',
                            array($_POST['vstupenka_typ'], $last_id));
                    }
                }

                $kap = $festival['zaplneno'];
                $kap= $kap+$_POST['vstupenky'];
                $pdo->query("UPDATE Festival SET zaplneno=${kap} WHERE id = ${festival['id']}"); // safe to use - capacity and festival is taken from database, so it can't cause issues
                alert("Uspesne rezervovano");
                print_footer();
                die();
            }
        }

        echo "<form method='post'>
<table>
    <tr>
        <th>Název</th><td>${festival['nazev']}</td>
    </tr>
    <tr>
        <th>Místo konáni</th><td>${festival['misto_konani']}</td>
    </tr>
    <tr>
        <th>Od</th><td>${festival['datum_od']}</td>
    </tr>
    <tr>
        <th>Do</th><td>${festival['datum_do']}</td>
    </tr>
    <tr>
        <th>Počet vstupenek</th><td><select name='vstupenky' id='vstupenky'>
            <option value='1'>1</option>
            <option value='2'>2</option>
            <option value='3'>3</option>
            <option value='4'>4</option>
            <option value='5'>5</option>
            <option value='6'>6</option>
            <option value='7'>7</option>
            <option value='8'>8</option>
            <option value='9'>9</option>
            <option value='10'>10</option>
        </select>
        </td>
    </tr>
    <tr>
        <th>Vstupenka</th><td><select name='vstupenka_typ' id='vstupenka_typ'>
            <option value='standard'>STANDARD</option>
            <option value='vip'>VIP</option>
        </select>
        </td>
    </tr>";
        if (!isset($_SESSION['user']))
            echo "
    <tr>
        <th>E-mailová adresa (povinná)</th><td><input name='email' type='email' id='email' required></td>
    </tr>";
        echo "
</table>
    <input type='submit' value='Odeslat'>
</form>";

        print_footer();
        die();
    }

    // reservations - only for logged in users
    if (isset($_SESSION['user']))
    {
        echo "<h1>Rezervace</h1>\n";
        $uzivatel = $pdo->query("SELECT id FROM Uzivatel WHERE prezdivka = '${_SESSION['user']}'")->fetch(); // safe to use - username is taken from database, so it can't cause issues
        $rows = $pdo->query("SELECT nazev, datum_od, datum_do, typ, COUNT(Vstupenka.id) AS pocet, stav FROM ((Rezervace_uzivatele JOIN Festival ON Rezervace_uzivatele.festival_id = Festival.id) JOIN Vstupenka ON Rezervace_uzivatele.id = Vstupenka.rezervace_uzivatele_id) WHERE uzivatel_id = ${uzivatel['id']} GROUP BY Vstupenka.rezervace_uzivatele_id ORDER BY Rezervace_uzivatele.id DESC"); // safe to use - username is taken from database, so it can't cause issues
        if ($rows->rowCount())
        {
            echo "<table>\n";
            echo "<tr>";
            foreach (array('Festival', 'Od', 'Do', 'Typ stupenky', 'Počet vstupenek', 'Stav rezervace') as $column)
                echo "<th>${column}</th>";
            echo "</tr>\n";
            while ($row = $rows->fetch())
            {
                echo "<tr>";
                foreach (array('nazev', 'datum_od', 'datum_do', 'typ', 'pocet', 'stav') as $column)
                    echo '<td>', $row[$column], '</td>';
                echo "</tr>\n";
            }
            echo "</table>\n";
        }
        else
            echo "<p><strong>Ještě jste si nic nekoupili</strong></p>\n";
    }

    print_footer();
?>
