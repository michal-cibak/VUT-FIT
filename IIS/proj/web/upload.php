<?php
    require 'functions.php';

    print_header('Nahrát logo');

    if (!has_rights_of('poradatel'))
    {
        print_footer();
        die();
    }

    if (isset($_GET['interpret']))
    {
        if (isset($_POST['submit']))
        {
            if (move_uploaded_file($_FILES["uploadfile"]["tmp_name"], "images/".basename($_FILES["uploadfile"]["name"])))
            {
                alert("Image uploaded successfully");
            }
            else
            {
                alert("Failed to upload image");
            }

            try {
                $stmt = $GLOBALS['pdo']->prepare('UPDATE Interpret SET logo_img = ? WHERE id = ?');
                $stmt->execute(array(basename($_FILES["uploadfile"]["name"]), $_GET['interpret']));
            }
            catch (PDOException $e) {
                echo '<h2>Spojení s databází selhalo: ' .$e->getMessage().'</h2>';
            }
        }
?>
    <form method="POST" enctype="multipart/form-data">
        <input type="file" name="uploadfile" id="uploadfile">
        <input type="submit" value="Upload" name="submit">
    </form>
<?php
    }

    print_footer();
?>
