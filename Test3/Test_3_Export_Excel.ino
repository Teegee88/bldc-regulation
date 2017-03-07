//always starts in line 0 and writes the thing written next to LABEL

void setup() {

  Serial.begin(9600); // the bigger number the better
  
  Serial.println("CLEARDATA"); //clears up any data left from previous projects
  
  Serial.println("LABEL,Acolumn,Bcolumn,..."); //always write LABEL, so excel knows the next things will be the names of the columns (instead of Acolumn you could write Time for instance)
  
  Serial.println("RESETTIMER"); //resets timer to 0

}

void loop() {

  Serial.print("DATA,TIME,TIMER,"); //writes the time in the first column A and the time since the measurements started in column B
  
  Serial.print(Adata);
  
  Serial.print(Bdata);
  
  Serial.println(...); //be sure to add println to the last command so it knows to go into the next row on the second run
  
  delay(100); //add a delay

}

//Declaration of the functions used
void Adata(){
  //Serial.read(............); 
}
