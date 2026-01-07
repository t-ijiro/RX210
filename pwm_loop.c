void loop()
{
    if(t == threshold)
    {
        matrix_write(0, 0, pwm_mode ? pixel_off : c);
        matrix_flush();
    }
    
    if(threshold == CYCLE_TIME)
    {
        t = 0; 
        threshold--;

        if(threshold == 0)
        {
            pwm_mode ^= 1;
            threshold = CYCLE_TIME;
        }
        
        matrix_write(0, 0, pwm_mode ? c : pixel_off);
        matrix_flush();
    }

    t++;
}
