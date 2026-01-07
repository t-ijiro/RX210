void loop()
{
    if(t == duty)
    {
        matrix_write(0, 0, pwm_mode ? pixel_off : c);
        matrix_flush();
    }
    
    if(t == RERIOD)
    {
        t = 0;
        duty--;

        if(duty == 0)
        {
            pwm_mode ^= 1;
            duty = PERIOD;
        }
        
        matrix_write(0, 0, pwm_mode ? c : pixel_off);
        matrix_flush();
    }

    t++;
}
