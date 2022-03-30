import random

def roll_dice():
    dice_num = input("How many dice would you like to roll?\n")
    output = ""

    for i in range(int(dice_num)):
        output += str(random.randint(1, 6)) + " "

    print(output)

try:
    roll_dice()
except:
    print("Please enter an integer for the number of dice.")
    roll_dice()
