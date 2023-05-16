function doGet(){
  var Cal = CalendarApp.getCalendarsByName('temperature')[0];
  var Now = new Date();
  var Future = new Date();
  Future.setDate(Now.getDate());
  Future.setMinutes(Now.getMinutes()+1);
  var events = Cal.getEvents(Now, Future);
  if(events.length == 0 || typeof events === 'undefined'){
    str=-999
  }else{
    str = events[0].getTitle()
  }
  Logger.log(str);
  return ContentService.createTextOutput(str);
}