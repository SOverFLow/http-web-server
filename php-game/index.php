<?php
require_once "./templates/header.php";
?>

<form method="post" action="register-players.php">
    <div class="welcome">
        <h1>Start playing Tic Tac Toe!</h1>
        <h2>Please fill in your names</h2>

        <div class="p-name">
            <label for="player-x"> Player (First)</label>
            <input type="text" id="player-x" name="player-x" required />
        </div>

        <div class="p-name">
            <label for="player-o"> Player (Sceond)</label>
            <input type="text" id="player-o" name="player-o" required />
        </div>

        <button type="submit">Start</button>
    </div>
</form>

<?php
require_once "./templates/footer.php";
