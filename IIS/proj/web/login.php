<?php
    require 'functions.php';

    if (isset($_SESSION['user']))
    {
        print_header('Přihlášeno');
        print_footer();
        die();
    }

    // try logging the user in if login form was sent and they aren't logged in already
    if (array_key_exists('username', $_POST) && array_key_exists('password', $_POST))
        if (login_user())
        {
            print_header('Přihlášeno');
            print_footer();
            die();
        }

    print_header('Přihlásit se');
?>
    <h1>Přihlásit se</h1>
    <form method="post">
        <label for="username">Uživatelské jméno</label><br>
        <input type="text" name="username" id="username" required><br>
        <label for="password">Heslo</label><br>
        <input type="password" name="password" id="password" required><br>
        <input type="submit" value="Příhlásit">
    </form>
<?php
    print_footer()
?>
