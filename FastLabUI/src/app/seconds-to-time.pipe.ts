import { Pipe, PipeTransform } from '@angular/core';

@Pipe({
  name: 'secondsToTime'
})
export class SecondsToTimePipe implements PipeTransform {

  transform(value: number): any {
    let seconds = new Date(null);
    seconds.setSeconds(value);
    let result = seconds.toISOString().substr(11,8);
    return result;
  }
}
