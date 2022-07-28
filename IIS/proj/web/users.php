<?php
    require 'functions.php';

    print_header('Uživatelé');

    // only 'administrator' can access this page
    if (!has_rights_of('administrator'))
    {
        print_footer();
        die();
    }

    // user editation
    if (array_key_exists('edit', $_GET))
    {
        if (array_key_exists('authorization', $_GET))
            edit_user($_GET['edit'], $_GET['authorization']);
        else
        {
            echo '<form>', "\n";
            echo '<input type="hidden" name="edit" value="', $_GET['edit'], '"/>', "\n";
            echo '<input type="radio" id="administrator" name="authorization" value="administrator">';
            echo '<label for="administrator">Administrátor</label><br>', "\n";
            echo '<input type="radio" id="poradatel" name="authorization" value="poradatel">';
            echo '<label for="poradatel">Pořadatel</label><br>', "\n";
            echo '<input type="radio" id="pokladni" name="authorization" value="pokladni">';
            echo '<label for="pokladni">Pokladní</label><br>', "\n";
            echo '<input type="radio" id="divak" name="authorization" value="divak">';
            echo '<label for="divak">Divák</label><br>', "\n";
            echo '<input type="submit" value="Upravit práva">', "\n";
            echo '</form>', "\n";
            print_footer();
            die();
        }
    }

    // user deletion
    if (array_key_exists('delete', $_GET))
        delete_user($_GET['delete']);

    // the webpage - list of users
    echo "<h1>Seznam uživatelů</h1>\n";
    print_table(
        array('ID', 'uživatelské jméno', 'E-mail', 'Oprávnění'),
        array('id', 'prezdivka', 'email', 'opravneni'),
        'Uzivatel',
        null,
        null,
        'users.php',
        'users.php'
    );

    echo "<h1>Přidat uživatele</h1>\n";
    echo "<p>\n";
    echo "<a href=register.php><button>Registrovat</button></a>\n";
    echo "</p>\n";

    print_footer();
?>
