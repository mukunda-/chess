"use server";

// Zod
import { z } from "zod";

// Ours - API
import { protectedProcedure } from "./shared";

export const echo = protectedProcedure(
  z.object({ message: z.string() }),
  async ({ input: { message } }): Promise<{ message: string }> => {
    return {
      message,
    };
  },
);
