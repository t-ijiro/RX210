void loop()
{
    if(t == duty)
    {
        matrix_write(0, 0, pwm_mode ? pixel_off : c);
        matrix_flush();
    }
    
    if(t == CYCLE_TIME)
    {
        t = 0;
        duty--;

        if(duty == 0)
        {
            pwm_mode ^= 1;
            duty = CYCLE_TIME;
        }
        
        matrix_write(0, 0, pwm_mode ? c : pixel_off);
        matrix_flush();
    }

    t++;
}
