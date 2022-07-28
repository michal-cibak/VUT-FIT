<?php
    require 'functions.php';

    print_header('Editace profilu');

    if (isset($_SESSION['user']))
    {
        if ($_POST)
        {
            try {
                $user = $pdo->query("SELECT * FROM Uzivatel WHERE prezdivka = '${_SESSION['user']}'")->fetch(); // safe to use - username is taken from database, so it can't cause issues
            }
            catch (PDOException $e) {
                echo '<h2>Spojení s databází selhalo: ' .$e->getMessage().'</h2>';
            }

            if (isset($_POST['email']) && $_POST['email'] !== '')
            {
                try {
                    $stmt = $GLOBALS['pdo']->prepare('UPDATE Uzivatel SET email = ? WHERE id = ?');
                    $stmt->execute(array($_POST['email'], $user['id']));
                }
                catch (PDOException $e) {
                    echo '<h2>Spojení s databází selhalo: ' .$e->getMessage().'</h2>';
                }
            }
            if (isset($_POST['password']) && $_POST['password'] !== '')
            {
                try {
                    $stmt = $GLOBALS['pdo']->prepare('UPDATE Uzivatel SET heslo = ? WHERE id = ?');
                    $stmt->execute(array(crypt($_POST['password']), $user['id']));
                }
                catch (PDOException $e) {
                    echo '<h2>Spojení s databází selhalo: ' .$e->getMessage().'</h2>';
                }
            }

            alert('Profil upraven');
        }

        echo "<h1>Uprav svůj profil</h1>\n";
        echo "<form method=post>
        <label for=email>E-mailová addresa<label><br>
        <input type=text name=email id=email><br>
        <label for=password>Heslo</label><br>
        <input type=password name=password id=password><br>
        <input type=submit value=Upravit>
        </form>\n";
        echo "<p>* prázdné pole = beze změny</p>\n";
    }

    print_footer();
?>
