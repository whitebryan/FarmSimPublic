# FarmSimPublic
Public repo of FarmSim project to be submitted as code samples

Current features :
Inventory system
	-Dragging and dropping
	-Combining stacks
	-Dropping items on the ground

Crop growth :
	-Grow based off real world minutes
	-Can set different seasons for growability
	-Can set growth times or amount of waterings needed
	-Can set amount harvested per harvest

Material harvesting / Breakable pieces:
	-Can require any tool type or none
	-Can require tool of certain tier
	-Gives a certain item and can give more quantity based off tiers above required tier
	-Regens based off real world minutes

Fishing :
	-Fish can only be caught in certain zones with optional extra fish based off time, weather, or seasons
	-Different difficulties based off tier of fish
	-Fishing zones can spawn that guarantee a certain tier or higher

Basic AI : 
	- Based off a schedule
		-Move to a different zone at defined periods of the day based off data table
		-While in zone roam at the defined range and interact with objects if stopped in front of one an allowed to interact with that type
	-Schedule can change based off weather

Save System : (Chests placed in editor will currently not agree with save system but that is a planned change coming soon)
	-Things saved
		-Player Inventory
		-Placed chests & their inventory
		-Crop growth progress and status
		-Harvestable regen status
		-Destructible status 
		-Time of day, Day of the year, Season, Weather
		-Recipes learned 
		-Rebound keybinds
		
